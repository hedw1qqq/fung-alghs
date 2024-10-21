//
// Created by ivglu on 13.10.2024.
//

#include "funcs.h"


int is_convex(int n, ...) {
    if (n <= 2)
        return 0;

    if (n < 4)
        return 1;


    va_list vals;
    Point all_points[n], a, b, c;

    va_start(vals, n);
    for (int i = 0; i < n; i++) {
        all_points[i] = va_arg(vals, Point);
    }
    va_end(vals);

    int sign = 0, prev;

    for (int i = 0; i < n; i++) {
        prev = sign;
        if (i < 2)
            a = all_points[n + i - 2], b = all_points[(n + i - 1) % n], c = all_points[i];
        else
            a = all_points[i - 2], b = all_points[i - 1], c = all_points[i];
        double dx1 = b.x - a.x, dy1 = b.y - a.y, dx2 = c.x - b.x, dy2 = c.y - b.y;
        double cross = dx1 * dy2 - dy1 * dx2;
        if (cross <= 0)
            sign = -1;
        else
            sign = 1;
        if (prev == 0)
            continue;
        if (sign != prev)
            return 0;
    }

    return 1;
}

Error_codes calculate_polynomial(double *result, double x, int n, ...) {
    va_list args;
    va_start(args, n);

    *result = 0;
    for (int i = n; i >= 0; --i) {
        double coeff = va_arg(args, double);
        *result += coeff * pow(x, i);
        if (isinf(*result) || isnan(*result)) {
            return OVERFLOW_ERROR;
        }
    }

    va_end(args);
    return OK;
}

int is_kaprekar(int number, int base) {
    long long square = (long long) number * number;
    int digits = 0;
    long long temp = square;

    while (temp > 0) {
        digits++;
        temp /= base;
    }

    for (int i = 1; i < digits; i++) {
        long long divisor = 1;
        for (int j = 0; j < i; j++) {
            divisor *= base;
        }
        long long right = square % divisor;
        long long left = square / divisor;

        if (right > 0 && left + right == number) {
            return 1;
        }
    }

    return 0;
}

int str_to_int_base(const char *str, int base) {
    return (int) strtol(str, NULL, base);
}

Error_codes find_kaprekar_numbers(int base, int num_args, ...) {
    if (num_args < 0 || base < 2 || base > 36)
        return ERROR_INVALID_INPUT;
    va_list args;
    va_start(args, num_args);


    for (int i = 0; i < num_args; i++) {
        const char *number_str = va_arg(args, const char*);
        int number = str_to_int_base(number_str, base);

        if (is_kaprekar(number, base)) {
            printf("%s (base %d) is a Kaprekar number\n", number_str, base);
        } else {
            printf("%s (base %d) is not a Kaprekar number\n", number_str, base);
        }
    }

    va_end(args);
    return OK;
}