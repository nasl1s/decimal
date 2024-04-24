#include "../../headers/s21_decimal.h"

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  s21_long_decimal value = s21_to_long(src);
  double tmp = 0;
  for (int i = s21_get_high_bit(value, BIT_MODE_DEFAULT); i >= 0; i--) {
    tmp += pow(2, i) * s21_get_bit(value, i);
  }
  tmp *= s21_get_sign(value) ? -1.0f : 1.0f;
  tmp *= pow(10, -s21_get_scale(value));
  *dst = (float)tmp;
  return 0;
}