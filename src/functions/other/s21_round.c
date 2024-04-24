#include "../../headers/s21_decimal.h"

int s21_round(s21_decimal value, s21_decimal *result) {
  int status = OK;
  s21_long_decimal tmp = s21_to_long(value);
  s21_decimal half = {{5, 0, 0, 65536}};
  if (s21_get_scale(tmp)) {
    if (s21_get_sign(tmp))
      status = s21_sub(value, half, &value);
    else
      status = s21_add(value, half, &value);
  }
  s21_truncate(value, result);
  return status;
}