#include "../../headers/s21_decimal.h"

int s21_truncate(s21_decimal value, s21_decimal *result) {
  s21_long_decimal tmp = s21_to_long(value);
  int scale = s21_get_scale(tmp);
  int sign = s21_get_sign(tmp);
  s21_set_scale(&tmp, 0);
  s21_mod(tmp, s21_pow10(scale), &tmp, BIT_MODE_DEFAULT);
  s21_set_sign(&tmp, sign);
  *result = s21_to_def(tmp);
  return 0;
}