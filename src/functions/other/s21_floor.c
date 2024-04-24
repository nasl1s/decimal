#include "../../headers/s21_decimal.h"

int s21_floor(s21_decimal value, s21_decimal *result) {
  int status = OK;
  s21_long_decimal buf = s21_to_long(value);
  s21_truncate(value, &value);
  s21_long_decimal tmp = s21_to_long(value);
  *result = s21_to_def(tmp);
  s21_decimal unit = DEC_UNIT;
  if (s21_get_scale(buf) && !s21_is_zero_value(buf) && s21_get_sign(tmp))
    status = s21_sub(*result, unit, result);
  return status;
}