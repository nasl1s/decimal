#include "../../headers/s21_decimal.h"

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_long_decimal val2 = s21_to_long(value_2);
  value_2 = s21_to_def(*s21_set_sign(&val2, s21_get_sign(val2) ? 0 : 1));
  return s21_add(value_1, value_2, result);  // TODO:replace with s21_negate();
                                             //поместится в одну строчку
}