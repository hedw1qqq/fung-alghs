#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPSILON 1e-6

enum errors {
    OK,
    INVALID_MEMORY,
    INVALID_INPUT
};


int is_finite_representation(double number, int base) {
    // Проверяем, является ли число целым
    if (fabs(number - round(number)) < EPSILON) {
        return 1;
    }

    // Находим знаменатель дроби
    long long denominator = 1;
    while (fabs(number * denominator - round(number * denominator)) >= EPSILON) {
        denominator *= base;

        if (denominator > 1000000000000LL) {
            return 0;
        }
    }

    // Проверяем, делится ли знаменатель нацело на базу
    while (denominator % base == 0) {
        denominator /= base;
    }

    return denominator == 1;
}

enum errors check_finite_representation(double **res, int base, int *idx, int count, ...) {
    if (base < 2) {
        return INVALID_INPUT;
    }

    va_list args;
    va_start(args, count);

    *res = (double *) malloc(sizeof(double) * count);
    if (*res == NULL) {
        return INVALID_MEMORY;
    }

    *idx = 0;

    for (int i = 0; i < count; ++i) {
        double number = va_arg(args, double);

        if (number > 1 - EPSILON || number < EPSILON) {
            continue;
        }

        if (is_finite_representation(number, base) == 1) {
            (*res)[(*idx)++] = number;
        }
    }

    if (*idx == 0) {
        free(*res);
        *res = NULL;
    }

    va_end(args);
    return OK;
}

int main() {
    double *res = NULL;
    int count = 0;
    enum errors err;

    // Test case 1: Base 3
    printf("Test case 1: Base 3\n");
    err = check_finite_representation(&res, 3, &count, 3, 1.0 / 3.0, 0.33333333333333333333333, 0.37);
    if (err == OK) {
        if (count == 0) {
            printf("No numbers have finite representation\n");
        } else {
            for (int i = 0; i < count; i++) {
                printf("%.6f has finite representation\n", res[i]);
            }
        }
        free(res);
    } else {
        printf("Error occurred\n");
    }
    printf("\n");

    // Test case 2: Base 2 (binary)
    printf("Test case 2: Base 2 (binary)\n");
    err = check_finite_representation(&res, 2, &count, 3, 0.5, 0.25, 0.1);
    if (err == OK) {
        if (count == 0) {
            printf("No numbers have finite representation\n");
        } else {
            for (int i = 0; i < count; i++) {
                printf("%.6f has finite representation\n", res[i]);
            }
        }
        free(res);
    } else {
        printf("Error occurred\n");
    }
    printf("\n");

    // Test case 3: Base 10 (decimal)
    printf("Test case 3: Base 10 (decimal)\n");
    err = check_finite_representation(&res, 10, &count, 3, 1, 1.0 / 3.0, 0.25);
    if (err == OK) {
        if (count == 0) {
            printf("No numbers have finite representation\n");
        } else {
            for (int i = 0; i < count; i++) {
                printf("%.6f has finite representation\n", res[i]);
            }
        }
        free(res);
    } else {
        printf("Error occurred\n");
    }
    printf("\n");

    // Test case 4: Base 16 (hexadecimal)
    printf("Test case 4: Base 16 (hexadecimal)\n");
    err = check_finite_representation(&res, 16, &count, 3, 0.0625, 0.125, 0.1);
    if (err == OK) {
        if (count == 0) {
            printf("No numbers have finite representation\n");
        } else {
            for (int i = 0; i < count; i++) {
                printf("%.6f has finite representation\n", res[i]);
            }
        }
        free(res);
    } else {
        printf("Error occurred\n");
    }
    printf("\n");

    // Test case 5: Invalid base
    printf("Test case 5: Invalid base\n");
    err = check_finite_representation(&res, 1, &count, 3, 0.5, 0.25, 0.1);
    if (err == INVALID_INPUT) {
        printf("Invalid base\n");
    } else {
        printf("Unexpected result\n");
    }

    return 0;
}