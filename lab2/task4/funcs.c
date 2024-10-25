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
        if (!sign) sign = (cross_prod > 0) ? 1 : -1;
        if (cross_prod * sign <= 0) {
            return 0;
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
        char *num_str = va_arg(args, char*);
        int is_kaprekar = 0;
        Error_codes err = is_kaprekar_base(num_str, base, &is_kaprekar);
        char *clean_num = remove_leading_zeros(num_str);

        if (err == OK) {
            if (is_kaprekar) {
                printf("%s is a Kaprekar number in base %d.\n", clean_num, base);
            } else {
                printf("%s is not a Kaprekar number in base %d.\n", clean_num, base);
            }
        } else {
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


long long str_to_decimal(const char *str, int base) {
    char *endptr;
    long long result = strtoll(str, &endptr, base);

    if (*endptr != '\0') {
        return -1;
    }

    return result;
}

Error_codes is_kaprekar_base_v2(const char *num_str, int base, int *result) {
    if (base < 2 || base > 36) {
        return ERR_INVALID_BASE;
    }

    if (num_str == NULL || result == NULL) {
        return ERROR_INVALID_INPUT;
    }

    *result = 0;
    Error_codes err = OK;

    if (strcmp(remove_leading_zeros(num_str), "1") == 0) {
        *result = 1;
        return OK;
    }

    char *square = multiply_base(num_str, num_str, base, &err);
    if (err != OK || square == NULL) {
        return err;
    }

    int square_len = strlen(square);

    for (int split = 1; split < square_len; split++) {

        char *left = malloc(split + 1);
        char *right = malloc(square_len - split + 1);

        if (!left || !right) {
            free(square);
            free(left);
            free(right);
            return ERR_ALLOCATION_FAIL;
        }

        strncpy(left, square, split);
        left[split] = '\0';

        strcpy(right, square + split);

        if (strcmp(remove_leading_zeros(right), "0") == 0 ||
            strcmp(remove_leading_zeros(left), "0") == 0) {
            free(left);
            free(right);
            continue;
        }

        char *sum;
        err = add_in_base(left, right, base, &sum);
        if (err != OK) {
            free(square);
            free(left);
            free(right);
            return err;
        }

        if (strcmp(remove_leading_zeros(sum), remove_leading_zeros(num_str)) == 0) {
            *result = 1;
            free(sum);
            free(left);
            free(right);
            free(square);
            return OK;
        }

        free(sum);
        free(left);
        free(right);
    }

    free(square);
    return OK;
}

void kaprekar_v2(int base, int n, ...) {
    if (n < 0) {
        printf("INVALID ARGS\n");
        return;
    }

    va_list args;
    va_start(args, n);

    printf("Checking Kaprekar numbers in base %d:\n", base);

    for (int i = 0; i < n; i++) {
        char *num_str = va_arg(args, char*);
        int is_kaprekar = 0;
        Error_codes err = is_kaprekar_base_v2(num_str, base, &is_kaprekar);
        char *clean_num = remove_leading_zeros(num_str);

        if (err == OK) {
            if (is_kaprekar) {
                printf("%s is a Kaprekar number in base %d.\n", clean_num, base);
            } else {
                printf("%s is not a Kaprekar number in base %d.\n", clean_num, base);
            }
        } else {
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
