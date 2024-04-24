#include "../../headers/s21_decimal.h"

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int status = OK;
  s21_long_decimal val1 = s21_to_long(value_1);
  s21_long_decimal val2 = s21_to_long(value_2);
  s21_long_decimal res = {0};

  //подготовка
  int sign = s21_get_sign(val1) ^ s21_get_sign(val2);
  int scale = s21_get_scale(val1) + s21_get_scale(val2);
  val1.bits[7] = 0;
  val2.bits[7] = 0;

  //умножение в столбик
  int high_bit = s21_get_high_bit(val2, BIT_MODE_DEFAULT);
  s21_long_decimal buf = val1;
  for (int i = 0; i <= high_bit; i++) {
    status = s21_bsum(res, s21_get_bit(val2, i) ? buf : s21_new_decimal(), &res,
                      BIT_MODE_LONG);  // TODO: replace with s21_add
    status = s21_left_shift(&buf, 1, BIT_MODE_LONG);
  }

  //нормализация значения
  int denorm_bits = s21_get_high_bit(res, BIT_MODE_LONG) - BIT_MODE_DEFAULT;
  s21_long_decimal remainder = {0};
  while (scale - 1 >= 0 && denorm_bits > 0) {
    remainder = s21_mod(res, s21_pow10(1), &res, BIT_MODE_LONG);
    scale--;
    denorm_bits = s21_get_high_bit(res, BIT_MODE_LONG) - BIT_MODE_DEFAULT;
  }
  if (denorm_bits <= 0) {
    //устанавка знака и степени
    s21_set_sign(&res, sign);
    s21_set_scale(&res, scale);

    //округление
    s21_long_decimal buf;
    // s21_long_decimal last_digit =
    s21_mod(res, s21_pow10(1), &buf, BIT_MODE_DEFAULT);
    // if (remainder.bits[0] == 5 && s21_is_zero(remainder)) {
    //   if (last_digit.bits[0] % 2 != 0)
    //     status = s21_bsum_unit(res, &res);  // TODO: replace with s21_add
    // } else
    if (remainder.bits[0] >= 5)
      status = s21_bsum_unit(res, &res);  // TODO: replace with s21_add

    //нормализация степени
    if (s21_get_scale(res) > 28 && !status)
      if ((status = s21_decrease_scale(&res, s21_get_scale(res) - 28)))
        s21_reset_value(&res);
    if (s21_is_zero_value(res)) s21_set_scale(&res, 0);
    *result = s21_to_def(res);
  } else
    status = sign ? ERROR_OVERFLOW_NEG : ERROR_OVERFLOW;
  return status;
}