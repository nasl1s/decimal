#include "../../headers/s21_decimal.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int status = OK;
  s21_long_decimal val1 = s21_to_long(value_1);
  s21_long_decimal val2 = s21_to_long(value_2);
  s21_long_decimal res = {0};

  //подготовка
  int sign1 = s21_get_sign(val1);
  int sign2 = s21_get_sign(val2);

  //выравниваем степень
  s21_align_scale(&val1, &val2);

  if (sign1 == sign2) {
    status = s21_bsum(val1, val2, &res, BIT_MODE_DEFAULT);
    if (status && s21_get_scale(val1) > 0) {
      int status1 = s21_decrease_scale(&val1, 1);
      int status2 = s21_decrease_scale(&val2, 1);
      if (!(status1 && status2)) {
        if (status1)
          s21_reset(&val1);
        else if (status2)
          s21_reset(&val2);
        status = s21_add(s21_to_def(val1), s21_to_def(val2), result);
        res = s21_to_long(*result);
      }
    }
    s21_set_sign(&res, sign1);
  } else if (sign1 < sign2) {
    if (s21_is_less_ext(val2, val1, BIT_MODE_DEFAULT))
      s21_bsub(val1, val2, &res, BIT_MODE_DEFAULT);
    else {
      s21_bsub(val2, val1, &res, BIT_MODE_DEFAULT);
      s21_set_sign(&res, 1);
    }
  } else if (sign1 > sign2) {
    if (s21_is_less_ext(val1, val2, BIT_MODE_DEFAULT))
      s21_bsub(val2, val1, &res, BIT_MODE_DEFAULT);
    else {
      s21_bsub(val1, val2, &res, BIT_MODE_DEFAULT);
      s21_set_sign(&res, 1);
    }
  }

  //выставляем степень
  s21_set_scale(&res, s21_get_scale(val1));
  // TODO:truncate??

  *result = s21_to_def(res);
  return status;
}