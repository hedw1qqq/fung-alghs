#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPSILON 1e-10

typedef enum {
    OK,
    INVALID_BASE,
    MEMORY_ERROR,
    INVALID_INPUT
} status_code;

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

status_code get_prime_factors(int number, int **factors, int *count, int *capacity) {
    *count = 0;

    for (int i = 2; i <= number; i++) {
        while (number % i == 0) {

            if (*count >= *capacity) {
                int new_capacity = *capacity * 2;
                int *new_factors = realloc(*factors, new_capacity * sizeof(int));
                if (new_factors == NULL) {
                    return MEMORY_ERROR;
                }
                *factors = new_factors;
                *capacity = new_capacity;
            }

            (*factors)[*count] = i;
            (*count)++;
            number /= i;
        }
    }

    return OK;
}

void reduce_fraction(int *numerator, int *denominator) {
    int divisor = gcd(abs(*numerator), abs(*denominator));
    *numerator /= divisor;
    *denominator /= divisor;
}

status_code fraction_approximation(double x, int *numerator, int *denominator) {
    *numerator = 1;
    *denominator = (int) round(1.0 / x);

    if (*denominator == 0) {
        return INVALID_INPUT;
    }

    double eps = EPSILON;
    double approximation = (double) *numerator / *denominator;
    double difference = approximation - x;

    while (fabs(difference) >= eps) {
        if (difference > 0) {
            int diffDenominator = (int) round(1.0 / difference);
            int newNumerator = *numerator * diffDenominator - *denominator;
            int newDenominator = *denominator * diffDenominator;
            *numerator = newNumerator;
            *denominator = newDenominator;
        } else {
            int diffDenominator = (int) round(1.0 / -difference);
            int newNumerator = *numerator * diffDenominator + *denominator;
            int newDenominator = *denominator * diffDenominator;
            *numerator = newNumerator;
            *denominator = newDenominator;
        }

        approximation = (double) *numerator / *denominator;
        difference = approximation - x;
    }

    reduce_fraction(numerator, denominator);
    return OK;
}

status_code is_prime_factors_base(int number, int base, int *is_valid) {
    if (number == 1) {
        *is_valid = 1;
        return OK;
    }

    int *den_factors = malloc(sizeof(int));
    if (den_factors == NULL) return MEMORY_ERROR;
    int den_capacity = 1;
    int den_count = 0;

    int *base_factors = malloc(sizeof(int));
    if (base_factors == NULL) {
        free(den_factors);
        return MEMORY_ERROR;
    }
    int base_capacity = 1;
    int base_count = 0;

    status_code result = get_prime_factors(number, &den_factors, &den_count, &den_capacity);
    if (result != OK) {
        free(den_factors);
        free(base_factors);
        return result;
    }

    result = get_prime_factors(base, &base_factors, &base_count, &base_capacity);
    if (result != OK) {
        free(den_factors);
        free(base_factors);
        return result;
    }

    *is_valid = 1;
    for (int i = 0; i < den_count; i++) {
        int found = 0;
        for (int j = 0; j < base_count; j++) {
            if (den_factors[i] == base_factors[j]) {
                found = 1;
                break;
            }
        }
        if (!found) {
            *is_valid = 0;
            break;
        }
    }

    free(den_factors);
    free(base_factors);

    return OK;
}

status_code has_finite_representation(int base, double number, int *is_valid) {
    if (number <= 0 || number >= 1) {
        *is_valid = 0;
        return OK;
    }

    int numerator = 0, denominator = 1;
    status_code status = fraction_approximation(number, &numerator, &denominator);
    if (status != OK) {
        return status;
    }

    return is_prime_factors_base(denominator, base, is_valid);
}

status_code check_finite_representation(double **result, int *count, int base, int n, ...) {
    if (n < 0) {
        return INVALID_INPUT;
    }
    if (base < 2 || base > 36) {
        return INVALID_BASE;
    }

    *result = malloc(sizeof(double) * n);
    if (*result == NULL) {
        return MEMORY_ERROR;
    }

    va_list args;
    va_start(args, n);

    *count = 0;

    for (int i = 0; i < n; i++) {
        double number = va_arg(args, double);
        int is_valid;
        status_code status = has_finite_representation(base, number, &is_valid);

        if (status != OK) {
            free(*result);
            *result = NULL;
            va_end(args);
            return status;
        }

        if (is_valid) {
            (*result)[*count] = number;
            (*count)++;
        }
    }

    va_end(args);

    if (*count == 0) {
        free(*result);
        *result = NULL;
    }

    return OK;
}

int main() {
    double *finite_numbers;
    int count;
    status_code status;

    printf("Testing base 2:\n");
    status = check_finite_representation(&finite_numbers, &count, 2, 4, 0.5, 0.25, 0.125, 0.1);
    if (status == OK) {
        for (int i = 0; i < count; i++) {
            printf("%.6f has finite representation\n", finite_numbers[i]);
        }
        if (finite_numbers != NULL) {
            free(finite_numbers);
        }
    } else {
        printf("Error: %d\n", status);
    }
    printf("\n");

    printf("Testing base 3:\n");
    status = check_finite_representation(&finite_numbers, &count, 3, 4, 0.33333333333333333333333, 1.0 / 9.0, 0.25,
                                         0.666666666666667);
    if (status == OK) {
        for (int i = 0; i < count; i++) {
            printf("%.6f has finite representation\n", finite_numbers[i]);
        }
        if (finite_numbers != NULL) {
            free(finite_numbers);
        }
    } else {
        printf("Error: %d\n", status);
    }
    printf("\n");

    printf("Testing base 10:\n");
    status = check_finite_representation(&finite_numbers, &count, 10, 4, 0.3, 1.0 / 9.0, 1.0 / 4.0,
                                         0.666666666666667);
    if (status == OK) {
        for (int i = 0; i < count; i++) {
            printf("%.6f has finite representation\n", finite_numbers[i]);
        }
        if (finite_numbers != NULL) {
            free(finite_numbers);
        }
    } else {
        printf("Error: %d\n", status);
    }
    printf("\n");

    printf("Testing invalid base:\n");
    status = check_finite_representation(&finite_numbers, &count, 1, 3, 0.5, 0.25, 0.1);
    if (status == OK) {
        for (int i = 0; i < count; i++) {
            printf("%.6f has finite representation\n", finite_numbers[i]);
        }
        if (finite_numbers != NULL) {
            free(finite_numbers);
        }
    } else {
        printf("Error: %d\n", status);
    }
    printf("\n");

    printf("Testing 36 base:\n");
    status = check_finite_representation(&finite_numbers, &count, 36, 4, 1.0 / 36.0, 1.0 / 72.0, 0.1, 1.0 / 2592);
    if (status == OK) {
        for (int i = 0; i < count; i++) {
            printf("%.6f has finite representation\n", finite_numbers[i]);
        }
        if (finite_numbers != NULL) {
            free(finite_numbers);
        }
    } else {
        printf("Error: %d\n", status);
    }
    printf("\n");

    return 0;
}
