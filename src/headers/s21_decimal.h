#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

#define OK 0
#define ERROR_CONVERTATION 1
#define ERROR_OVERFLOW 1
#define ERROR_UNDERFLOW 2
#define ERROR_OVERFLOW_NEG 2
#define ERROR_ZERO_DIV 3
#define ERROR 4

#define DEC_MAX 79228162514264337593543950335.0
#define DEC_MIN -79228162514264337593543950335.0
#define DEC_UNIT   \
  {                \
    { 1, 0, 0, 0 } \
  }
#define DEC_LONG_UNIT          \
  {                            \
    { 1, 0, 0, 0, 0, 0, 0, 0 } \
  }

#define BIT_MODE_DEFAULT 95
#define BIT_MODE_LONG 223

enum index { low, mid, high, scale };

/// @brief decimal struct
typedef struct {
  int bits[4];
} s21_decimal;

typedef struct {
  int bits[8];
} s21_long_decimal;

// GETTERS AND SETTERS
int s21_get_bit(s21_long_decimal value, int pos);
void s21_set_bit(s21_long_decimal* value, int pos, int bit);
int s21_get_sign(s21_long_decimal value);
s21_long_decimal* s21_set_sign(s21_long_decimal* value, int sign);
int s21_get_scale(s21_long_decimal value);
s21_long_decimal* s21_set_scale(s21_long_decimal* value, char scale);

// BINARY
int s21_get_high_bit(s21_long_decimal value, int mode);
void s21_reverse(s21_long_decimal* value);
int s21_left_shift(s21_long_decimal* value, int shift, int mode);
// void s21_right_shift(s21_long_decimal* value, int shift);
int s21_bsum(s21_long_decimal value1, s21_long_decimal value2,
             s21_long_decimal* result, int high_bit);
void s21_bsub(s21_long_decimal value_1, s21_long_decimal value_2,
              s21_long_decimal* result, int mode);

// COMMON
s21_long_decimal s21_new_decimal();
void s21_reset(s21_long_decimal* value);
void s21_reset_value(s21_long_decimal* value);
int s21_is_zero_value(s21_long_decimal value);
int s21_is_zero(s21_long_decimal value);
int s21_mul10(s21_long_decimal* value, int mode);
s21_long_decimal s21_pow10(int scale);
int s21_increase_scale(s21_long_decimal* value, int shift);
int s21_decrease_scale(s21_long_decimal* value, int shift);
void s21_align_scale(s21_long_decimal* value_1, s21_long_decimal* value_2);
int s21_bsum_unit(s21_long_decimal value1, s21_long_decimal* result);
int s21_is_equal_value(s21_long_decimal value_1, s21_long_decimal value_2,
                       int mode);
int s21_is_less_ext(s21_long_decimal value_1, s21_long_decimal value_2,
                    int mode);

// CONVERT
s21_long_decimal s21_to_long(s21_decimal value);
s21_decimal s21_to_def(s21_long_decimal value);

// PRINT (DEBUG FUNCTIONS)
// void s21_print(s21_decimal value);
// void s21_println(s21_decimal value);
// void s21_print_value(s21_decimal value);
// void s21_print_long(s21_long_decimal value);
// void s21_println_long(s21_long_decimal value);

// ARITHMETICS
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);

s21_long_decimal s21_mod(s21_long_decimal value_1, s21_long_decimal value_2,
                         s21_long_decimal* result, int mode);

// CONVERTION
int s21_from_int_to_decimal(int src, s21_decimal* dst);
int s21_from_float_to_decimal(float src, s21_decimal* dst);
int s21_from_decimal_to_int(s21_decimal src, int* dst);
int s21_from_decimal_to_float(s21_decimal src, float* dst);

// COMPARSION
int comparison(s21_decimal val1, s21_decimal val2);
int s21_is_equal(s21_decimal val1, s21_decimal val2);
int s21_is_greater_or_equal(s21_decimal val1, s21_decimal val2);
int s21_is_greater(s21_decimal val1, s21_decimal val2);
int s21_is_less_or_equal(s21_decimal val1, s21_decimal val2);
int s21_is_less(s21_decimal val1, s21_decimal val2);
int s21_is_not_equal(s21_decimal val1, s21_decimal val2);

// OTHER
int s21_negate(s21_decimal value, s21_decimal* result);
int s21_truncate(s21_decimal value, s21_decimal* result);
int s21_floor(s21_decimal value, s21_decimal* result);
int s21_round(s21_decimal value, s21_decimal* result);

#endif