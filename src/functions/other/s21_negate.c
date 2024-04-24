#include "../../headers/s21_decimal.h"

int s21_negate(s21_decimal value, s21_decimal *result) {
  s21_long_decimal tmp = s21_to_long(value);
  s21_set_sign(&tmp, !s21_get_sign(tmp));
  *result = s21_to_def(tmp);
  return 0;
}