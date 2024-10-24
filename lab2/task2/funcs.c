//
// Created by ivglu on 11.10.2024.
//

#include "funcs.h"

ErrorCode geometric_mean(double *result, int count, ...) {
    if (count <= 0) {
        return INVALID_INPUT;
    }
    va_list args;
    va_start(args, count);
    *result = 1;

    for (int i = 0; i < count; ++i) {
        *result *= va_arg(args, double);
        if (isnan(*result) || isinf(*result)) {
            va_end(args);
            return OVERFLOW_ERROR;
        }
    }

    *result = pow(*result, 1.0 / count);
    va_end(args);
    return SUCCESS;
}

ErrorCode fast_pow(double base, int p, double *result) {
    ErrorCode status;
    if (p < 0) {
        status = fast_pow(1.0 / base, -1 * p, result);
        return status;
    }
    if (p == 0) {
        *result = 1.0;
        return SUCCESS;
    }
    if (p % 2 == 0) {
        status = fast_pow(base, p / 2, result);
        if ((isnan(*result) || isinf(*result))) {
            return OVERFLOW_ERROR;
        }
        *result *= *result;
    } else {
        status = fast_pow(base, p - 1, result);
        if ((isnan(*result) || isinf(*result))) {
            return OVERFLOW_ERROR;
        }
        *result *= base;
    }
    return status;
}