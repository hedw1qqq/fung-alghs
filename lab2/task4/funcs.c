//
// Created by ivglu on 13.10.2024.
//

#include "funcs.h"

Error_codes add_in_base(const char *num1, const char *num2, int base, char **result) {
    int len1 = strlen(num1);
    int len2 = strlen(num2);
    int max_len = (len1 > len2) ? len1 : len2;

    *result = calloc(max_len + 2, sizeof(char));
    if (!*result) {
        return ERR_ALLOCATION_FAIL;
    }

    Error_codes err = OK;
    int carry = 0;
    int i = len1 - 1, j = len2 - 1, k = max_len;

    while (i >= 0 || j >= 0 || carry) {
        int sum = carry;

        if (i >= 0) {
            int digit = char_to_int(num1[i--], base, &err);
            if (err != OK) {
                free(*result);
                return err;
            }
            sum += digit;
        }

        if (j >= 0) {
            int digit = char_to_int(num2[j--], base, &err);
            if (err != OK) {
                free(*result);
                return err;
            }
            sum += digit;
        }

        char result_char;
        Error_codes char_err = int_to_char(sum % base, &result_char);
        if (char_err != OK) {
            free(*result);
            return char_err;
        }
        (*result)[k--] = result_char;
        carry = sum / base;
    }

    char *final_result = *result + k + 1;

    while (*final_result == '0' && *(final_result + 1) != '\0') {
        final_result++;
    }

    if (final_result != *result) {
        memmove(*result, final_result, strlen(final_result) + 1);
        char *temp = realloc(*result, strlen(*result) + 1);
        if (temp == NULL) {
            free(*result);
            return ERR_ALLOCATION_FAIL;
        }
        *result = temp;
    }

    return OK;
}


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

    if (n < 4)
        return 1;
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

        if (cross_prod != 0) {
            int current_sign = (cross_prod > 0) ? 1 : -1;
            if (sign == 0) {
                sign = current_sign;
            } else if (sign != current_sign) {
                return 0;
            }
        }
    }

    return 1;
}


Error_codes calculate_polynomial(double *result, double x, int n, ...) {
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

int char_to_int(char c, int base, Error_codes *err) {
    if (c >= '0' && c <= '9') {
        int value = c - '0';
        if (value < base) return value;
    } else if (c >= 'A' && c <= 'Z') {
        int value = c - 'A' + 10;
        if (value < base) return value;
    } else if (c >= 'a' && c <= 'z') {
        int value = c - 'a' + 10;
        if (value < base) return value;
    }
    *err = ERR_INVALID_CHAR;
    return -1;
}

Error_codes int_to_char(int n, char *result) {
    if (n >= 0 && n <= 9) {
        *result = '0' + n;
        return OK;
    } else if (n >= 10 && n < 36) {
        *result = 'A' + (n - 10);
        return OK;
    }
    return ERR_INVALID_CHAR;
}

char *multiply_base(const char *num1, const char *num2, int base, Error_codes *err) {
    int len1 = strlen(num1);
    int len2 = strlen(num2);
    int result_len = len1 + len2;
    int *result = calloc(result_len, sizeof(int));

    if (result == NULL) {
        *err = ERR_ALLOCATION_FAIL;
        return NULL;
    }

    for (int i = len1 - 1; i >= 0; i--) {
        int digit1 = char_to_int(num1[i], base, err);
        if (*err != OK) {
            free(result);
            return NULL;
        }
        for (int j = len2 - 1; j >= 0; j--) {
            int digit2 = char_to_int(num2[j], base, err);
            if (*err != OK) {
                free(result);
                return NULL;
            }
            result[i + j + 1] += digit1 * digit2;
            result[i + j] += result[i + j + 1] / base;
            result[i + j + 1] %= base;
        }
    }

    char *result_str = malloc(result_len + 1);
    if (result_str == NULL) {
        free(result);
        *err = ERR_ALLOCATION_FAIL;
        return NULL;
    }

    int index = 0;
    int leading_zero = 1;
    for (int i = 0; i < result_len; i++) {
        if (result[i] == 0 && leading_zero) {
            continue;
        }
        leading_zero = 0;
        char result_char;
        Error_codes char_err = int_to_char(result[i], &result_char);
        if (char_err != OK) {
            free(result_str);
            free(result);
            *err = char_err;
            return NULL;
        }
        result_str[index++] = result_char;
    }
    result_str[index] = '\0';

    if (index == 0) {
        result_str[0] = '0';
        result_str[1] = '\0';
    }

    free(result);
    return result_str;
}

char *remove_leading_zeros(const char *num_str) {
    while (*num_str == '0' && *(num_str + 1) != '\0') {
        num_str++;
    }
    return (char *) num_str;
}

Error_codes is_kaprekar_base(const char *num_str, int base, int *result) {
    if (base < 2 || base > 36) return ERR_INVALID_BASE;

    Error_codes err = OK;
    *result = 0;

    char *cleaned_num_str = remove_leading_zeros(num_str);
    char *square = multiply_base(cleaned_num_str, cleaned_num_str, base, &err);
    if (err != OK) return err;

    int num_len = strlen(cleaned_num_str);
    int square_len = strlen(square);

    int right_len = num_len;
    if (square_len <= right_len) {
        free(square);
        return OK;
    }

    char *right_part = malloc(right_len + 1);
    if (right_part == NULL) {
        free(square);
        return ERR_ALLOCATION_FAIL;
    }
    strcpy(right_part, &square[square_len - right_len]);

    int left_len = square_len - right_len;
    char *left_part = malloc(left_len + 1);
    if (left_part == NULL) {
        free(square);
        free(right_part);
        return ERR_ALLOCATION_FAIL;
    }
    strncpy(left_part, square, left_len);
    left_part[left_len] = '\0';

    // Сложение частей в столбик
    char *sum = NULL;
    err = add_in_base(left_part, right_part, base, &sum);
    if (err != OK) {
        free(square);
        free(right_part);
        free(left_part);
        return err;
    }

    // Сравнение суммы с исходным числом
    if (strcmp(sum, cleaned_num_str) == 0) {
        *result = 1;
    }

    // Освобождение памяти
    free(square);
    free(right_part);
    free(left_part);
    free(sum);

    return OK;
}

void check_kaprekar_numbers_in_base(int base, int n, ...) {
    if (n < 0) {
        printf("INVALID ARGS");
        return;
    }
    va_list args;
    va_start(args, n);

    printf("Checking Kaprekar numbers in base %d:\n", base);

    for (int i = 0; i < n; i++) {
        char *num_str = va_arg(args, char*);
        int is_kaprekar = 0;
        Error_codes err = is_kaprekar_base(num_str, base, &is_kaprekar);
        char *num_str_no_leading_zeros = remove_leading_zeros(num_str);
        if (err == OK) {
            if (is_kaprekar) {
                printf("%s is a Kaprekar number in base %d.\n", num_str_no_leading_zeros, base);
            } else {
                printf("%s is not a Kaprekar number in base %d.\n", num_str_no_leading_zeros, base);
            }
        } else {
            if (err == ERR_INVALID_CHAR) {
                printf("Error: '%s' contains invalid characters for base %d.\n", num_str_no_leading_zeros, base);
            } else if (err == ERR_INVALID_BASE) {
                printf("Error: base %d is not supported.\n", base);
            } else if (err == ERR_ALLOCATION_FAIL) {
                printf("Error: memory allocation failed for '%s'.\n", num_str_no_leading_zeros);
            }
        }
    }
    printf("\n");
    va_end(args);
}


long long str_to_decimal(const char *str, int base) {
    char *endptr;
    long long result = strtoll(str, &endptr, base);

    if (*endptr != '\0') {
        return -1;
    }

    return result;
}

int is_kaprekar_base_v2(long long n, int base) {
    if (n == 1) return 1;

    long long square = n * n;

    int num_digits = 0;
    long long temp = n;
    while (temp > 0) {
        temp /= base;
        num_digits++;
    }

    long long divisor = pow(base, num_digits);
    long long left = square / divisor;
    long long right = square % divisor;


    return (left + right == n);
}

void kaprekar_v2(int base, int n, ...) {
    va_list args;
    va_start(args, n);

    printf("Checking Kaprekar numbers in base %d:\n", base);

    for (int i = 0; i < n; i++) {
        char *str = va_arg(args, char*);
        long long num = str_to_decimal(str, base);

        if (num == -1) {
            printf("Invalid number '%s' for base %d.\n", str, base);
            continue;
        }

        if (is_kaprekar_base_v2(num, base)) {
            str = remove_leading_zeros(str);
            printf("%s is a Kaprekar number in base %d.\n", str, base);
        } else {
            str = remove_leading_zeros(str);
            printf("%s is NOT a Kaprekar number in base %d.\n", str, base);
        }
    }

    va_end(args);
}
