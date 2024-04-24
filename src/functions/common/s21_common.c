#include "../../headers/s21_decimal.h"

//===========================================
// GETTERS AND SETTERS
//===========================================

int s21_get_bit(s21_long_decimal value, int pos) {
  return 1u & (value.bits[pos / 32] >> pos % 32);
}

void s21_set_bit(s21_long_decimal* value, int pos, int bit) {
  if (bit)
    value->bits[pos / 32] |= 1u << pos % 32;
  else if (!bit)
    value->bits[pos / 32] &= ~(1u << pos % 32);
}

int s21_get_sign(s21_long_decimal value) { return s21_get_bit(value, 255); }

s21_long_decimal* s21_set_sign(s21_long_decimal* value, int sign) {
  s21_set_bit(value, 255, sign);
  return value;
}

int s21_get_scale(s21_long_decimal value) {
  return (char)(value.bits[7] >> 16);
}

s21_long_decimal* s21_set_scale(s21_long_decimal* value, char scale) {
  int sign = s21_get_sign(*value);
  value->bits[7] = scale << 16;
  s21_set_sign(value, sign);
  return value;
}

//===========================================
// BINARY
//===========================================

int s21_get_high_bit(s21_long_decimal value, int mode) {
  int bit = mode;
  while (bit >= 0 && !s21_get_bit(value, bit)) bit--;
  return bit;
}

void s21_reverse(s21_long_decimal* value) {
  for (int i = 0; i < 7; i++) value->bits[i] = ~value->bits[i];
}

int s21_left_shift(s21_long_decimal* value, int shift, int mode) {
  int i = mode;
  int status =
      s21_get_high_bit(*value, mode) + shift <= mode ? OK : ERROR_OVERFLOW;
  for (; i - shift >= 0; i--)
    s21_set_bit(value, i, s21_get_bit(*value, i - shift));
  for (; i >= 0; i--) s21_set_bit(value, i, 0);
  return status;
}

// void s21_right_shift(s21_long_decimal* value, int shift) {
//   int i = 0;
//   for (; i + shift <= BIT_MODE_LONG; i++)
//     s21_set_bit(value, i, s21_get_bit(*value, i + shift));
//   for (; i <= BIT_MODE_LONG; i++) s21_set_bit(value, i, 0);
// }

int s21_bsum(s21_long_decimal value1, s21_long_decimal value2,
             s21_long_decimal* result, int high_bit) {
  int bit = 0;
  // s21_reset(result);
  for (int i = 0; i <= high_bit; i++) {
    int sum = s21_get_bit(value1, i) + s21_get_bit(value2, i) + bit;
    s21_set_bit(result, i, sum % 2);
    bit = sum >= 2 ? 1 : 0;
  }
  return bit;
}

void s21_bsub(s21_long_decimal value_1, s21_long_decimal value_2,
              s21_long_decimal* result, int mode) {
  s21_reset(result);
  if (!s21_is_equal_value(value_1, value_2, mode)) {
    int high_bit = s21_get_high_bit(value_1, mode);
    s21_long_decimal buf = value_2;
    s21_reverse(&buf);
    s21_bsum(value_1, buf, result, high_bit);
    s21_long_decimal unit = DEC_LONG_UNIT;
    s21_bsum(*result, unit, result, high_bit);
  }
}

//===========================================
// COMMON
//===========================================

s21_long_decimal s21_new_decimal() {
  s21_long_decimal new = {0};
  return new;
}

void s21_reset(s21_long_decimal* value) {
  for (int i = 0; i < 8; i++) value->bits[i] = 0;
}

void s21_reset_value(s21_long_decimal* value) {
  for (int i = 0; i < 7; i++) value->bits[i] = 0;
}

int s21_is_zero_value(s21_long_decimal value) {
  int res = 1;
  for (int i = 0; i < 7 && res; i++) res = value.bits[i] == 0;
  return res;
}

int s21_is_zero(s21_long_decimal value) {
  int res = 1;
  for (int i = 0; i < 8 && res; i++) res = value.bits[i] == 0;
  return res;
}

int s21_mul10(s21_long_decimal* value, int mode) {
  int status = OK;
  s21_long_decimal buf1 = *value, buf2 = *value;
  if (!s21_left_shift(&buf1, 1, mode) && !s21_left_shift(&buf2, 3, mode)) {
    s21_long_decimal buf3 = *value;
    if (!(status = s21_bsum(buf1, buf2, &buf3, mode))) *value = buf3;
  } else
    status = ERROR_OVERFLOW;
  return status;
}

s21_long_decimal s21_pow10(int scale) {
  s21_long_decimal result = s21_new_decimal();
  result.bits[0] = 1;
  for (int i = 0; i < scale; i++) s21_mul10(&result, BIT_MODE_LONG);
  return result;
}

//пока что можно увеличивать только на 1 степень за раз
int s21_increase_scale(s21_long_decimal* value, int shift) {
  int scale = s21_get_scale(*value);
  int status = OK;
  if (scale + shift < 29 &&
      !s21_mul10(value, BIT_MODE_DEFAULT))  // TODO: replace with mul
    s21_set_scale(value, scale + shift);
  else
    status = ERROR_OVERFLOW;
  return status;
}

int s21_decrease_scale(s21_long_decimal* value, int shift) {
  int status = OK;
  int scale = s21_get_scale(*value);
  s21_decimal buf = s21_to_def(*value);
  buf.bits[3] = s21_get_sign(*value) ? 0x801C0000 : 0x001C0000;
  if (scale - shift >= 0 && !s21_div(buf, s21_to_def(s21_pow10(shift)), &buf)) {
    if (s21_is_zero_value(s21_to_long(buf)))
      status = ERROR_UNDERFLOW;
    else {
      *value = s21_to_long(buf);
      s21_set_scale(value, scale - shift);
    }
  }  // else
  //   status = ERROR_OVERFLOW;
  return status;
}

void s21_align_scale(s21_long_decimal* value_1, s21_long_decimal* value_2) {
  int scale = s21_get_scale(*value_1) - s21_get_scale(*value_2);
  if (scale > 0) {
    while (scale != 0 && !s21_increase_scale(value_2, 1)) scale--;
    if (scale) s21_decrease_scale(value_1, scale);
  } else if (scale < 0) {
    while (scale != 0 && !s21_increase_scale(value_1, 1)) scale++;
    if (scale) s21_decrease_scale(value_2, -scale);
  }
}

// TODO: delete this
int s21_bsum_unit(s21_long_decimal value1, s21_long_decimal* result) {
  s21_long_decimal unit = DEC_LONG_UNIT;
  return s21_bsum(value1, unit, result, BIT_MODE_DEFAULT);
}

int s21_is_equal_value(s21_long_decimal value_1, s21_long_decimal value_2,
                       int mode) {
  (void)mode;
  int res = 1;
  for (int i = 0; (i < 7) && res; i++) res = value_1.bits[i] == value_2.bits[i];
  return res;
}

int s21_is_less_ext(s21_long_decimal value_1, s21_long_decimal value_2,
                    int mode) {
  int res = 0;
  for (int i = mode; i >= 0 && !res; i--)
    res = s21_get_bit(value_2, i) - s21_get_bit(value_1, i);
  return res > 0 ? 1 : 0;
}

//===========================================
// COMPARTION
//===========================================

int simple_eq(s21_long_decimal value_1,
              s21_long_decimal value_2) {  // првоеряем на равенство
  int diff = 0;
  for (int i = 191; i >= 0; i--) {
    diff = s21_get_bit(value_1, i) - s21_get_bit(value_2, i);
    if (diff != 0) break;
  }
  return diff;
}

int comparison(s21_decimal val1, s21_decimal val2) {
  s21_long_decimal value_1 = s21_to_long(val1), value_2 = s21_to_long(val2),
                   zero = {0};  // Создаем временные переменные value_1, value_2
                                // и zero для сравнения.
  int sign_1 = s21_get_sign(value_1), sign_2 = s21_get_sign(value_2),
      res = 0;  // Получаем знаки sign_1 и sign_2 для val1 и val2.
  if (sign_1 ==
      sign_2) {  // Если знаки равны, нормализуем числа и сравниваем их.
    s21_align_scale(
        &value_1,
        &value_2);  // Если знаки равны, нормализуем числа и сравниваем их.
    res = simple_eq(value_1, value_2);
    if (sign_1) res = -res;  // Если числа отрицательные, инвертируем результат
  } else {
    if (simple_eq(value_1, zero) == 0 &&
        simple_eq(value_2, zero) == 0) {  // Если оба числа равны нулю
      res = 0;
    } else {
      if (sign_1)  //Если первое число отрицательное, значит оно меньше, поэтому
                   //возвращаем -1.
        res = -1;
      else if (sign_2)
        res = 1;
    }
  }
  return res;
}

//===========================================
// CONVERT
//===========================================

s21_long_decimal s21_to_long(s21_decimal value) {
  s21_long_decimal buf = {
      {value.bits[0], value.bits[1], value.bits[2], 0, 0, 0, 0, value.bits[3]}};
  return buf;
}

s21_decimal s21_to_def(s21_long_decimal value) {
  s21_decimal buf = {
      {value.bits[0], value.bits[1], value.bits[2], value.bits[7]}};
  return buf;
}

//===========================================
// PRINT
//===========================================

// void s21_print(s21_decimal value) {
//   s21_long_decimal buf = s21_to_long(value);
//   for (int i = 127; i >= 0; i--) {
//     printf("%d", s21_get_bit(buf, i));
//     if (i > 0 && i % 32 == 0) printf(".");
//   }
// }

// void s21_print_long(s21_long_decimal value) {
//   for (int i = 255; i >= 0; i--) {
//     printf("%d", s21_get_bit(value, i));
//     if (i > 0 && i % 32 == 0) printf(".");
//   }
// }

// void s21_println(s21_decimal value) {
//   s21_print(value);
//   printf("\n");
// }

// void s21_println_long(s21_long_decimal value) {
//   s21_print_long(value);
//   printf("\n");
// }

// void s21_print_value(s21_decimal value) {
//   s21_long_decimal val = s21_to_long(value);
//   printf("{0x%08X, 0x%08X, 0x%08X, 0x%08X} | val = ", val.bits[0],
//   val.bits[1],
//          val.bits[2], val.bits[7]);
//   char str[32] = "";
//   str[0] = s21_get_sign(val) ? '-' : '+';
//   int point = 30 - s21_get_scale(val);
//   point = point > 29 ? 0 : point;
//   val.bits[7] = 0;
//   if (point > 1) str[point] = point == 30 ? '\0' : ',';
//   for (int i = 30; i >= 1; i--) {
//     if (i != point) {
//       s21_long_decimal buf = s21_mod(val, s21_pow10(1), &val,
//       BIT_MODE_DEFAULT); str[i] = buf.bits[0] + '0';
//     }
//   }
//   str[31] = '\0';
//   printf("%s\n", str);
// }