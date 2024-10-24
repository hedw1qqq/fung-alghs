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

    long long abs_num = llabs(num);
    int sign = (num < 0) ? 1 : 0;
    int len = 0;

    do {
        len++;
        abs_num /= base;
    } while (abs_num != 0);

    len += sign; 

    char *result = (char *) malloc((len + 1) * sizeof(char));
    if (result == NULL) {
        return NULL;
    }

    result[len] = '\0';
    abs_num = llabs(num); 


    do {
        int remainder = abs_num % base;
        result[--len] = (remainder < 10) ? remainder + '0' : remainder - 10 + 'A';
        abs_num /= base;
    } while (abs_num != 0);
    if (sign) {
        result[0] = '-';
    }

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
                printf("Invalid base input\n");
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

    char *num_str = (char *) malloc(sizeof(long long) * 8 + 1);
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
        int capacity = sizeof(long long) * 8 + 1;

        num_str[0] = '\0';
        while ((ch = getchar()) != EOF && (isspace(ch) || ch == '\t' || ch == '\n'));
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
            free(max_str);
            max_str = strdup(num_str);
        }
    }

    if (max_str == NULL) {
        printf("No valid numbers entered.\n");
        free(num_str);
        return 0;
    }

    const char *max_str_without_zeros = strip_leading_zeros(max_str);
    printf("Max number (input base %d): %s\n", base, max_str_without_zeros);
    printf("Max number in base 10: %lld\n", max_number);

    char *base9 = convert_from_decimal(max_number, 9);
    if (base9 != NULL) {
        printf("Max number in base 9: %s\n", base9);
    } else {
        printf("Error converting number to base 9.\n");
    }

    char *base18 = convert_from_decimal(max_number, 18);
    if (base18 != NULL) {
        printf("Max number in base 18: %s\n", base18);
    } else {
        printf("Error converting number to base 18.\n");
    }

    char *base27 = convert_from_decimal(max_number, 27);
    if (base27 != NULL) {
        printf("Max number in base 27: %s\n", base27);
    } else {
        printf("Error converting number to base 27.\n");
    }

    char *base36 = convert_from_decimal(max_number, 36);
    if (base36 != NULL) {
        printf("Max number in base 36: %s\n", base36);
    } else {
        printf("Error converting number to base 36.\n");
    }

    free(num_str);
    free(max_str);
    free(base9);
    free(base18);
    free(base27);
    free(base36);

    return SUCCESS;
}
