#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

typedef enum {
    OK ,
    ERROR_INVALID_BASE,
    ERROR_INVALID_INPUT,
    ERROR_MEMORY_ALLOCATION
} ErrorCode;

int char_to_digit(char c) {
    if (isdigit(c)) return c - '0';
    if (isalpha(c)) return toupper(c) - 'A' + 10;
    return -1;
}

char digit_to_char(int digit) {
    if (digit < 10) return digit + '0';
    return digit - 10 + 'A';
}

ErrorCode add_in_base(const char* num1, const char* num2, int base, char** result) {
    int len1 = strlen(num1);
    int len2 = strlen(num2);
    int max_len = (len1 > len2) ? len1 : len2;

    *result = calloc(max_len + 2, sizeof(char));
    if (!*result) return ERROR_MEMORY_ALLOCATION;

    int carry = 0;
    int i = len1 - 1, j = len2 - 1, k = max_len;

    while (i >= 0 || j >= 0 || carry) {
        int sum = carry;
        if (i >= 0) {
            int digit = char_to_digit(num1[i--]);
            if (digit == -1 || digit >= base) {
                free(*result);
                return ERROR_INVALID_INPUT;
            }
            sum += digit;
        }
        if (j >= 0) {
            int digit = char_to_digit(num2[j--]);
            if (digit == -1 || digit >= base) {
                free(*result);
                return ERROR_INVALID_INPUT;
            }
            sum += digit;
        }

        (*result)[k--] = digit_to_char(sum % base);
        carry = sum / base;
    }

    char* final_result = *result + k + 1;
    while (*final_result == '0' && *(final_result + 1) != '\0') {
        final_result++;
    }

    if (final_result != *result) {
        memmove(*result, final_result, strlen(final_result) + 1);
        *result = realloc(*result, strlen(*result) + 1);
    }

    return OK;
}

ErrorCode sum_in_base(int base, int count, char** result, ...) {
    if (base < 2 || base > 36 || count < 1) return ERROR_INVALID_BASE;

    va_list args;
    va_start(args, result);

    *result = strdup("0");
    if (!*result) return ERROR_MEMORY_ALLOCATION;

    for (int i = 0; i < count; i++) {
        const char* num = va_arg(args, const char*);
        char* temp;
        ErrorCode error = add_in_base(*result, num, base, &temp);
        if (error != OK) {
            free(*result);
            va_end(args);
            return error;
        }
        free(*result);
        *result = temp;
    }

    va_end(args);
    return OK;
}

int main() {
    char* result;
    ErrorCode error;

    error = sum_in_base(16, 3, &result, "A5", "BB", "14C");
    if (error == OK) {
        printf("Sum in base 16: %s\n", result);
        free(result);
    } else {
        printf("Error: %d\n", error);
    }

    error = sum_in_base(2, 4, &result, "1111", "1111", "1111", "1111");
    if (error == OK) {
        printf("Sum in base 2: %s\n", result);
        free(result);
    } else {
        printf("Error: %d\n", error);
    }

    error = sum_in_base(36, 3, &result, "XYZ", "ABC", "123");
    if (error == OK) {
        printf("Sum in base 36: %s\n", result);
        free(result);
    } else {
        printf("Error: %d\n", error);
    }
    error = sum_in_base(1, 2, &result, "1", "2");
    if (error == OK) {
        printf("Sum in base 36: %s\n", result);
        free(result);
    } else {
        printf("Error: %d\n", error);
    }

    error = sum_in_base(10, 2, &result, "123", "ABC");
    if (error == OK) {
        printf("Sum in base 36: %s\n", result);
        free(result);
    } else {
        printf("Error: %d\n", error);
    }

    return 0;
}