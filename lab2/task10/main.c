#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    OK,
    INVALID_INPUT,
    INVALID_MEMORY,
} errors;


double evaluate_polynomial(double argument, double *coefs, int max_power) {
    double result = 0;
    double current_power = 1;
    for (int i = 0; i <= max_power; i++) {
        result += current_power * coefs[i];
        current_power *= argument;
    }
    return result;
}

errors calculate_polynomial(double a, double **coefficients, int degree, ...) {
    if (degree < 0) return INVALID_INPUT;

    (*coefficients) = (double *) malloc((degree + 1) * sizeof(double));
    if ((*coefficients) == NULL) {
        return INVALID_MEMORY;
    }

    int n = degree;
    double *start_coefficients = (double *) malloc(sizeof(double) * (degree + 1));
    if (start_coefficients == NULL) {
        free(*coefficients);
        return INVALID_MEMORY;
    }

    va_list args;
    va_start(args, degree);
    for (int i = 0; i <= degree; i++) {
        double coefficient = va_arg(args, double);
        start_coefficients[i] = coefficient;
    }
    va_end(args);

    double multiply = 1.0;
    int curr_power = degree;
    for (int i = 0; i <= n; i++) {
        (*coefficients)[i] = evaluate_polynomial(a, start_coefficients, curr_power);
        if (i > 1) {
            multiply *= i;
            (*coefficients)[i] /= multiply;
        }

        curr_power--;
        for (int j = 0; j <= curr_power; ++j) {
            double b = start_coefficients[j + 1];
            start_coefficients[j] = b * (j + 1);
        }
    }

    free(start_coefficients);
    return OK;
}

int main() {
    double *result;
    errors err;

    // Тест 1: f(x) = -2 + x - 3x^2 + x^4, a = 3
    printf("Test 1: f(x) = -2 + x - 3x^2 + x^4, a = 3\nExpected: 55.000000 91.000000 51.000000 12.000000 1.000000\n");
    err = calculate_polynomial(3.0, &result, 4, -2.0, 1.0, -3.0, 0.0, 1.0);
    if (err == OK) {
        for (int i = 0; i <= 4; ++i) {
            printf("%lf ", result[i]);
        }
        printf("\n");
    } else {
        printf("error\n");
    }
    free(result);

    // Тест 2: f(x) = 1 - 2x + x^2, a = 2
    printf("\nTest 2: f(x) = 1 - 2x + x^2, a = 2\nExpected: 1.000000 2.000000 1.000000\n");
    err = calculate_polynomial(2.0, &result, 2, 1.0, -2.0, 1.0);
    if (err == OK) {
        for (int i = 0; i <= 2; ++i) {
            printf("%lf ", result[i]);
        }
        printf("\n");
    } else {
        printf("error\n");
    }
    free(result);

    // Тест 3: f(x) = 4x^3 + 3x^2 + 2x + 1, a = 0
    printf("\nTest 3: f(x) = 4x^3 + 3x^2 + 2x + 1, a = 0\nExpected: 1.000000 2.000000 3.000000 4.000000\n");
    err = calculate_polynomial(0.0, &result, 3, 1.0, 2.0, 3.0, 4.0);
    if (err == OK) {
        for (int i = 0; i <= 3; ++i) {
            printf("%lf ", result[i]);
        }
        printf("\n");
    } else {
        printf("error\n");
    }
    free(result);

    // Тест 4: f(x) = 5x + 6, a = -1
    printf("\nTest 4: f(x) = 5x + 6, a = -1\nExpected: 1.000000 5.000000\n");
    err = calculate_polynomial(-1.0, &result, 1, 6.0, 5.0);
    if (err == OK) {
        for (int i = 0; i <= 1; ++i) {
            printf("%lf ", result[i]);
        }
        printf("\n");
    } else {
        printf("error\n");
    }
    free(result);

    // Тест 5: f(x) = x^5, a = 1
    printf("\nTest 5: f(x) = x^5, a = 1\nExpected: 1.000000 5.000000 10.000000 10.000000 5.000000 1.000000\n");
    err = calculate_polynomial(1.0, &result, 5, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    if (err == OK) {
        for (int i = 0; i <= 5; ++i) {
            printf("%lf ", result[i]);
        }
        printf("\n");
    } else {
        printf("error\n");
    }
    free(result);

    return 0;
}