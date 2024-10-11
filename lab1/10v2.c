#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

typedef enum {
    SUCCESS,
    ERROR_INVALID_BASE,
    ERROR_MEMORY_ERROR,
    ERROR_INVALID_INPUT,
    ERROR_OVERFLOW
} ErrorCode;

ErrorCode str_to_int(const char *number_str, int *number) {
    int is_negative = 0;
    int j = 0;
    *number = 0;

    if (number_str[0] == '-') {
        is_negative = 1;
        j = 1;
    }

    for (; number_str[j]; ++j) {
        if (number_str[j] >= '0' && number_str[j] <= '9') {
            if (*number > (INT_MAX - (number_str[j] - '0')) / 10) {
                return ERROR_OVERFLOW;
            }
            *number = *number * 10 + (number_str[j] - '0');
        } else {
            return ERROR_INVALID_INPUT;
        }
    }

    if (is_negative) {
        *number = -*number;
        if (*number < INT_MIN) {
            return ERROR_OVERFLOW;
        }
    }

    return SUCCESS;
}

ErrorCode convert_to_decimal(const char *num, int base, long long *result) {
    *result = 0;
    int sign = 1;

    for (int i = 0; num[i] != '\0'; i++) {
        char c = toupper(num[i]);
        int digit;

        if (i == 0 && num[i] == '-') {
            sign = -1;
            continue;
        }

        if (isdigit(c)) {
            digit = c - '0';
        } else if (isalpha(c)) {
            digit = c - 'A' + 10;
        } else {
            return ERROR_INVALID_INPUT;
        }

        if (digit >= base) {
            return ERROR_INVALID_INPUT;
        }

        if (*result > (LLONG_MAX - digit) / base) {
            return ERROR_OVERFLOW;
        }

        *result = *result * base + digit;
    }

    *result *= sign;
    return SUCCESS;
}

char *convert_from_decimal(long long num, int base) {
    if (base < 2 || base > 36) {
        return NULL;
    }

    char *buffer = (char *) malloc(65);
    char *ptr = buffer + 64;
    *ptr = '\0';

    int sign = (num < 0) ? -1 : 1;
    num = llabs(num);

    do {
        int remainder = num % base;
        *--ptr = (remainder < 10) ? remainder + '0' : remainder - 10 + 'A';
        num /= base;
    } while (num != 0);

    if (sign == -1) {
        *--ptr = '-';
    }


    char *result = (char *) malloc(strlen(ptr) + 1);
    strcpy(result, ptr);

    free(buffer);

    return result;
}


const char *strip_leading_zeros(const char *num) {
    while (*num == '0') {
        num++;
    }
    return (*num == '\0') ? "0" : num;
}

int main() {
    int base;
    char s1[4];
    int i = 0;
    int c;

    printf("Enter the base of the number system (2 to 36): ");

    while ((c = getchar()) != '\n' && c != EOF) {
        if (i > 2) {
            printf("Invalid base input\n");
            return ERROR_INVALID_INPUT;
        }

        if (isspace(c)) {
            if (i > 0) {
                printf("Invalid base input");
                return ERROR_INVALID_INPUT;
            }
            continue;
        }

        s1[i++] = c;
    }
    s1[i] = '\0';

    ErrorCode code = str_to_int(s1, &base);
    if (code != SUCCESS || base < 2 || base > 36) {
        printf("Invalid base input\n");
        return ERROR_INVALID_BASE;
    }

    char *num_str = (char *) malloc(65 * sizeof(char));  // Начальный буфер
    if (num_str == NULL) {
        printf("Memory allocation failed\n");
        return ERROR_MEMORY_ERROR;
    }

    long long max_number = LLONG_MIN;
    char *max_str = NULL;

    while (1) {
        printf("Enter a number (or 'Stop' to end): ");
        int idx = 0;
        int ch;
        int capacity = 65;

        num_str[0] = '\0';
        while ((ch = getchar()) != EOF && (isspace(ch) || ch == '0' || ch =='\t' || ch == '\n'));
        if (ch != '\n' && ch != EOF) {
            num_str[idx++] = ch;
        }
        while ((ch = getchar()) != '\n' && ch != EOF) {
            if (idx >= capacity) {
                capacity *= 2;
                char *new_str = (char *) realloc(num_str, capacity);
                if (new_str == NULL) {
                    printf("Memory allocation failed\n");
                    free(num_str);
                    return ERROR_MEMORY_ERROR;
                }
                num_str = new_str;
            }
            num_str[idx++] = ch;
        }
        num_str[idx] = '\0';

        if (strcmp(num_str, "Stop") == 0) {
            break;
        }

        long long decimal_value;
        code = convert_to_decimal(num_str, base, &decimal_value);
        if (code == ERROR_INVALID_INPUT) {
            printf("Invalid input detected, skipping this number.\n");
            continue;
        } else if (code == ERROR_OVERFLOW) {
            printf("Overflow detected, skipping this number.\n");
            continue;
        }

        if (decimal_value > max_number) {
            max_number = decimal_value;
            if (max_str != NULL) {
                free(max_str);
            }
            max_str = strdup(num_str);  // Копируем новое максимальное значение
        }
    }

    if (max_str == NULL) {
        printf("No valid numbers entered.\n");
        free(num_str);
        return 0;
    }

    const char *max_str_withiout_zero = strip_leading_zeros(max_str);
    printf("Max number (input base %d): %s\n", base, max_str_withiout_zero);
    printf("Max number in base 10: %lld\n", max_number);

    // Преобразование числа в другие системы счисления
    char *base9 = convert_from_decimal(max_number, 9);
    char *base18 = convert_from_decimal(max_number, 18);
    char *base27 = convert_from_decimal(max_number, 27);
    char *base36 = convert_from_decimal(max_number, 36);

    // Вывод результатов
    printf("Max number in base 9: %s\n", base9);
    printf("Max number in base 18: %s\n", base18);
    printf("Max number in base 27: %s\n", base27);
    printf("Max number in base 36: %s\n", base36);

    // Освобождение памяти
    free(num_str);
    free(max_str);
    free(base9);
    free(base18);
    free(base27);
    free(base36);

    return SUCCESS;
}
