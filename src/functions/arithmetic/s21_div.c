#include "../../headers/s21_decimal.h"

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int status = OK;
  s21_long_decimal val1 = s21_to_long(value_1);
  s21_long_decimal val2 = s21_to_long(value_2);
  s21_long_decimal res = {0};
  if (!s21_is_zero_value(val2)) {
    //подготовка
    int sign = s21_get_sign(val1) ^ s21_get_sign(val2);
    int scale = s21_get_scale(val1) - s21_get_scale(val2);
    val1.bits[7] = 0;
    val2.bits[7] = 0;

    //первичное деление для получения целого и установка знака и степени
    s21_long_decimal remainder = s21_mod(val1, val2, &res, BIT_MODE_DEFAULT);
    s21_set_sign(&res, sign);
    s21_set_scale(&res, scale);

    //посимвольная запись дробной части
    s21_long_decimal buf;
    while (!status && !s21_is_zero(remainder) &&
           !s21_mul10(&remainder, BIT_MODE_LONG) &&
           !s21_increase_scale(&res, 1)) {
      buf = s21_new_decimal();
      remainder = s21_mod(remainder, val2, &buf, BIT_MODE_LONG);
      status = s21_bsum(res, buf, &res,
                        BIT_MODE_DEFAULT);  // TODO: replace with s21_add
    }

    //округление
    s21_long_decimal last_digit =
        s21_mod(res, s21_pow10(1), &buf, BIT_MODE_DEFAULT);
    s21_long_decimal next_digit = s21_new_decimal();
    remainder = s21_mod(remainder, val2, &next_digit, BIT_MODE_LONG);
    if (next_digit.bits[0] == 5 && s21_is_zero(remainder)) {
      if (last_digit.bits[0] % 2 != 0)
        status = s21_bsum_unit(res, &res);  // TODO: replace with s21_add
    } else if (next_digit.bits[0] >= 5)
      status = s21_bsum_unit(res, &res);  // TODO: replace with s21_add

    //нормализация степени
    while (s21_get_scale(res) < 0 && !status)
      status = s21_increase_scale(&res, 1);
    if (status && sign) status = ERROR_OVERFLOW_NEG;
  } else
    status = ERROR_ZERO_DIV;
  *result = s21_to_def(res);
  return status;
}