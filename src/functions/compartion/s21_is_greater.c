#include "../../headers/s21_decimal.h"

int s21_is_greater(s21_decimal val1, s21_decimal val2) {
  int res = comparison(val1, val2);
  return (res == 1) ? TRUE : FALSE;
}