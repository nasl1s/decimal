#include "../../headers/s21_decimal.h"

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int status = OK;
  s21_long_decimal val = {0};
  if (fabsf(src) < 1e-28 || fabsf(src) > DEC_MAX || isinf(src) || isnan(src)) {
    status = ERROR_CONVERTATION;
  } else {
    s21_set_sign(&val, signbit(src));
    double buf = fabs(src);
    double expo = floor(log10(buf));
    double mant = buf / pow(10, (int)expo);
    val.bits[0] = (int)(mant * pow(10, 7));
    s21_set_scale(&val, 28);
    s21_div(s21_to_def(val), s21_to_def(s21_pow10(1)), dst);
    val = s21_to_long(*dst);
    s21_set_scale(&val, (int)(6 - expo));
    if (s21_get_scale(val) > 28 && !status)
      status = s21_decrease_scale(&val, s21_get_scale(val) - 28);
    while (s21_get_scale(val) < 0 && !status)
      status = s21_increase_scale(&val, 1);
  }
  *dst = s21_to_def(val);
  return status;
}