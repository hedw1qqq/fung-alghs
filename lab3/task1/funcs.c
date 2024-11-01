//
// Created by ivglu on 26.10.2024.
//

#include "funcs.h"

int sum(int num1, int num2) {
    int res = num1 ^ num2;
    unsigned int carry = (num1 & num2) << 1;

    while (carry) {
        int tmp = res;
        res = res ^ carry;
        carry = (unsigned int)(tmp & carry) << 1;
    }
    return res;
}

int negative(int n) {
    return sum(~n, 1);
}

int sum_with_negative(int a, int b) {
    return sum(a, negative(b));
}

void reverse_string(char *str, int length) {
    int start = 0;
    int end = sum_with_negative(length, 1);

    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;

        start = sum(start, 1);
        end = sum_with_negative(end, 1);
    }
}

int is_power_of_two(int n) {
    return (n > 1) && ((n & (sum_with_negative(n, 1))) == 0);
}

errors decimal_into_powern(int num, int base, char **res, int *capacity, int *count) {
    if (!is_power_of_two(base) || base > 36) {
        return INVALID_INPUT;
    }

    *count = 0;
    int is_negative = 0;

    if (num < 0) {
        is_negative = 1;
        num = negative(num);
    }

    do {
        if (*count >= *capacity) {
            *capacity = sum(*capacity, *capacity);
            char *for_realloc = (char *) realloc(*res, *capacity * sizeof(char));
            if (for_realloc == NULL) {
                return INVALID_MEMORY;
            }
            *res = for_realloc;
        }

        int digit = num % base;
        if (digit < 10) {
            (*res)[*count] = sum('0', digit);
        } else {
            (*res)[*count] = sum('A', sum_with_negative(digit, 10));
        }

        *count = sum(*count, 1);
        num /= base;
    } while (num > 0);

    if (is_negative) {
        if (*count >= *capacity) {
            *capacity = sum(*capacity, *capacity);
            char *for_realloc = (char *) realloc(*res, *capacity * sizeof(char));
            if (for_realloc == NULL) {
                return INVALID_MEMORY;
            }
            *res = for_realloc;
        }
        (*res)[*count] = '-';
        *count = sum(*count, 1);
    }

    reverse_string(*res, *count);

    if (*count >= *capacity) {
        *capacity = sum(*capacity, 1);
        char *for_realloc = (char *) realloc(*res, *capacity * sizeof(char));
        if (for_realloc == NULL) {
            return INVALID_MEMORY;
        }
        *res = for_realloc;
    }
    (*res)[*count] = '\0';
    return OK;
}
