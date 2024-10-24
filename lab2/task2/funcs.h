//
// Created by ivglu on 11.10.2024.
//

#ifndef TASK2_FUNCS_H
#define TASK2_FUNCS_H

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

typedef enum {
    SUCCESS,
    INVALID_INPUT,
    OVERFLOW_ERROR
} ErrorCode;

ErrorCode geometric_mean(double *result, int count, ...);

ErrorCode fast_pow(double base, int exponent, double *result);

#endif //TASK2_FUNCS_H
