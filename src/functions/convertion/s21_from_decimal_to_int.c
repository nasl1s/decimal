#include "../../headers/s21_decimal.h"

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int status = OK;
  s21_long_decimal buf = s21_to_long(src);
  int sign = s21_get_sign(buf);
  s21_mod(buf, s21_pow10(s21_get_scale(buf)), &buf, BIT_MODE_DEFAULT);
  s21_set_sign(&buf, sign);
  src = s21_to_def(buf);
  status = src.bits[1] != 0 || src.bits[2] != 0 || ((1u & (src.bits[0] >> 31)));
  if (!status) *dst = src.bits[0] * (sign ? -1 : 1);
  return status;
}