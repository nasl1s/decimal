#include "../../headers/s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  s21_decimal buf = {{src < 0 ? -src : src, 0, 0, src < 0 ? 0x80000000 : 0}};
  *dst = buf;
  return OK;
}