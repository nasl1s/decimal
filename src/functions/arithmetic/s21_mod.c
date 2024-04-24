#include "../../headers/s21_decimal.h"

s21_long_decimal s21_mod(s21_long_decimal value_1, s21_long_decimal value_2,
                         s21_long_decimal *result, int mode) {
  //подготовка
  s21_reset(result);
  s21_long_decimal remainder = s21_new_decimal();
  int high_bit = s21_get_high_bit(value_1, mode);

  //деление в столбик
  while (high_bit >= 0) {
    s21_left_shift(&remainder, 1, mode);
    s21_set_bit(&remainder, 0, s21_get_bit(value_1, high_bit));
    if (s21_is_less_ext(remainder, value_2, mode)) {
      s21_left_shift(result, 1, mode);
      s21_set_bit(result, 0, 0);
    } else {
      s21_bsub(remainder, value_2, &remainder, mode);
      s21_left_shift(result, 1, mode);
      s21_set_bit(result, 0, 1);
    }
    high_bit--;
  }

  return remainder;
}