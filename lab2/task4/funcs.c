//
// Created by ivglu on 13.10.2024.
//

#include "funcs.h"

double cross_product(Point p1, Point p2, Point p3) {
    double x1 = p2.x - p1.x;
    double y1 = p2.y - p1.y;
    double x2 = p3.x - p2.x;
    double y2 = p3.y - p2.y;
    return x1 * y2 - y1 * x2;
}


int is_convex_polygon(int n, ...) {
    if (n <= 2)
        return 0;

    va_list vals;
    Point points[n];

    va_start(vals, n);
    for (int i = 0; i < n; i++) {
        points[i] = va_arg(vals, Point);
    }
    va_end(vals);


    int sign = 0;

    for (int i = 0; i < n; i++) {
        Point p1 = points[i];
        Point p2 = points[(i + 1) % n];
        Point p3 = points[(i + 2) % n];

        double cross_prod = cross_product(p1, p2, p3);
        if (!sign) sign = (cross_prod > 0) ? 1 : -1;
        if (cross_prod * sign <= 0) {
            return 0;
        }
    }
    return 1;
}


Error_codes calculate_polynomial(double* result, double x, int n, ...) {
    if (n < 0) {
        return ERROR_INVALID_INPUT;
    }
    va_list args;
    va_start(args, n);

    *result = 0;
    for (int i = 0; i <= n; i++) {
        double coeff = va_arg(args, double);
        *result += coeff * pow(x, i);
        if (isinf(*result) || isnan(*result)) {
            return OVERFLOW_ERROR;
        }
    }

    va_end(args);
    return OK;
}

char* remove_leading_zeros(const char* num_str) {
    while (*num_str == '0' && *(num_str + 1) != '\0') {
        num_str++;
    }
    return (char*)num_str;
}

Error_codes is_kaprekar_base(const char* num_str, int base, int* result) {
    if (base < 2 || base > 36) {
        return ERR_INVALID_BASE;
    }

    if (num_str == NULL || result == NULL) {
        return ERROR_INVALID_INPUT;
    }

    *result = 0;
    Error_codes err = OK;


    long long number = str_to_decimal(num_str, base);
    if (number == -1) {
        return ERR_INVALID_CHAR;
    }


    if (number == 1) {
        *result = 1;
        return OK;
    }


    if (number <= 0) {
        return ERROR_INVALID_INPUT;
    }

    unsigned long long square = (unsigned long long) number * number;

    int square_digits = 0;
    unsigned long long temp = square;
    while (temp > 0) {
        temp /= base;
        square_digits++;
    }

    for (int i = 1; i < square_digits; i++) {
        unsigned long long divisor = 1;
        for (int j = 0; j < i; j++) {
            divisor *= base;
        }

        unsigned long long right = square % divisor;
        unsigned long long left = square / divisor;

        if (right == 0 || left == 0) {
            continue;
        }

        if (left + right == number) {
            *result = 1;
            return OK;
        }
    }

    return OK;
}

void check_kaprekar_numbers_in_base(int base, int n, ...) {
    if (n < 0) {
        printf("INVALID ARGS\n");
        return;
    }

    va_list args;
    va_start(args, n);

    printf("Checking Kaprekar numbers in base %d:\n", base);

    for (int i = 0; i < n; i++) {
        char* num_str = va_arg(args, char*);
        int is_kaprekar = 0;
        Error_codes err = is_kaprekar_base(num_str, base, &is_kaprekar);
        char* clean_num = remove_leading_zeros(num_str);

        if (err == OK) {
            if (is_kaprekar) {
                printf("%s is a Kaprekar number in base %d.\n", clean_num, base);
            }
            else {
                printf("%s is not a Kaprekar number in base %d.\n", clean_num, base);
            }
        }
        else {
            switch (err) {
            case ERR_INVALID_BASE:
                printf("Error: base %d is not supported.\n", base);
                break;
            case ERR_INVALID_CHAR:
                printf("Error: Invalid character in number '%s' for base %d.\n", clean_num, base);
                break;
            case ERR_ALLOCATION_FAIL:
                printf("Error: Memory allocation failed for '%s'.\n", clean_num);
                break;
            default:
                printf("Error occurred while processing '%s'.\n", clean_num);
                break;
            }
        }
    }

    printf("\n");
    va_end(args);
}


long long str_to_decimal(const char* str, int base) {
    char* endptr;
    long long result = strtoll(str, &endptr, base);

    if (*endptr != '\0') {
        return -1;
    }

    return result;
}
