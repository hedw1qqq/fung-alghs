//
// Created by ivglu on 13.10.2024.
//

#ifndef TASK4_FUNCS_H
#define TASK4_FUNCS_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef enum {
    OK,
    ERROR_INVALID_INPUT,
    OVERFLOW_ERROR,
    ERR_INVALID_BASE,
    ERR_ALLOCATION_FAIL,
    ERR_INVALID_CHAR
} Error_codes;
typedef struct {
    double x, y;
} Point;

double cross_product(Point p1, Point p2, Point p3);

int is_convex_polygon(int n, ...);

Error_codes add_in_base(const char *num1, const char *num2, int base, char **result);


Error_codes calculate_polynomial(double *result, double x, int n, ...);

int char_to_int(char c, int base, Error_codes *err);

Error_codes int_to_char(int n, char *result);

char *multiply_base(const char *num1, const char *num2, int base, Error_codes *err);

char *remove_leading_zeros(const char *num_str);

Error_codes is_kaprekar_base_v2(const char *num_str, int base, int *result);

void check_kaprekar_numbers_in_base(int base, int n, ...);

void kaprekar_v2(int base, int n, ...);
Error_codes is_kaprekar_base(const char *num_str, int base, int *result);

long long str_to_decimal(const char *str, int base);

#endif //TASK4_FUNCS_H
