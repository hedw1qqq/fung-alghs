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
    OVERFLOW_ERROR
} Error_codes;
typedef struct {
    double x, y;
} Point;

double cross_product(Point a, Point b, Point c);

Error_codes is_convex_polygon(int num_points, ...);

Error_codes calculate_polynomial(double *result, double x, int n, ...);

int is_kaprekar(int number, int base);

int str_to_int_base(const char *str, int base);

void find_kaprekar_numbers(int base, int num_args, ...);

#endif //TASK4_FUNCS_H
