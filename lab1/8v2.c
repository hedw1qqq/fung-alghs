#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

typedef enum {
    SUCCESS,
    ERROR_INVALID_INPUT,
    ERROR_OVERFLOW,
    ERROR_MEMORY_FAIL,
    ERROR_FILE_FAIL,
    ERROR_SAME_FILE_NAMES
} ErrorCode;

const char *strip_leading_zeros(const char *num) {
    if (num[0] == '-') {
        num++;  // Skip the minus sign
        while (*num == '0' && *(num + 1) != '\0') {
            num++;
        }
        return (*(num - 1) == '-') ? num - 1 : num;  // Reattach the minus sign if needed
    }
    while (*num == '0' && *(num + 1) != '\0') {
        num++;
    }
    return num;
}

ErrorCode convert_to_decimal(const char *num, int base, long long *result) {
    *result = 0;
    int sign = 1;

    if (*num == '-') {
        sign = -1;
        num++;
    }

    if (*num == '\0') {
        return ERROR_INVALID_INPUT;
    }

    for (int i = 0; num[i] != '\0'; i++) {
        char c = toupper(num[i]);
        int digit;

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

int determine_min_base(const char *num) {
    int min_base = 2;
    if (*num == '-') num++; // Skip the negative sign if present

    if (*num == '\0') {
        return ERROR_INVALID_INPUT;
    }

    for (int i = 0; num[i] != '\0'; i++) {
        char c = toupper(num[i]);
        int digit;

        if (isdigit(c)) {
            digit = c - '0';
        } else if (isalpha(c)) {
            digit = c - 'A' + 10;
        } else {
            return ERROR_INVALID_INPUT;
        }

        if (digit + 1 > min_base) {
            min_base = digit + 1;
        }
    }
    return min_base;
}

char *read_lexeme_by_char(FILE *file) {
    size_t size = 32;
    size_t len = 0;
    char *buffer = malloc(size);

    if (!buffer) {
        return NULL;
    }

    int c;
    while ((c = fgetc(file)) != EOF && isspace(c));

    if (c == EOF) {
        free(buffer);
        return NULL;
    }

    do {
        buffer[len++] = c;

        if (len == size) {
            size *= 2;
            char *new_buffer = realloc(buffer, size);
            if (!new_buffer) {
                free(buffer);
                return NULL;
            }
            buffer = new_buffer;
        }

        c = fgetc(file);
    } while (c != EOF && !isspace(c));

    buffer[len] = '\0';

    if (len == 0) {
        free(buffer);
        return NULL;
    }

    return buffer;
}

void flush_buffer(FILE *output, char *buffer, size_t *output_len) {
    fprintf(output, "%s", buffer);
    *output_len = 0;
    buffer[0] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return ERROR_FILE_FAIL;
    }

    if (strcmp(argv[1], argv[2]) == 0) {
        printf("Error: Input and output files cannot be the same.\n");
        return ERROR_SAME_FILE_NAMES;
    }

    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        printf("Error: Could not open input file.\n");
        return ERROR_FILE_FAIL;
    }

    FILE *output_file = fopen(argv[2], "w");
    if (!output_file) {
        printf("Error: Could not open or create output file.\n");
        fclose(input_file);
        return ERROR_FILE_FAIL;
    }

    char *output_buffer = malloc(10000);
    if (!output_buffer) {
        fclose(input_file);
        fclose(output_file);
        return ERROR_MEMORY_FAIL;
    }
    size_t output_len = 0;
    char *buffer;

    while ((buffer = read_lexeme_by_char(input_file)) != NULL) {
        const char *trimmed_num = strip_leading_zeros(buffer);

        if (strcmp(trimmed_num, "-") == 0) {
            free(buffer);
            continue;
        }

        int min_base = determine_min_base(trimmed_num);
        if (min_base == ERROR_INVALID_INPUT) {
            output_len += sprintf(output_buffer + output_len, "Invalid input: %s\n", buffer);
            free(buffer);
            continue;
        }

        long long decimal_value;
        ErrorCode conversion_result = convert_to_decimal(trimmed_num, min_base, &decimal_value);
        if (conversion_result == ERROR_OVERFLOW) {
            output_len += sprintf(output_buffer + output_len, "Overflow: %s\n", buffer);
            free(buffer);
            continue;
        } else if (conversion_result == ERROR_INVALID_INPUT) {
            output_len += sprintf(output_buffer + output_len, "Invalid input: %s\n", buffer);
            free(buffer);
            continue;
        }

        output_len += sprintf(output_buffer + output_len, "%s %d %lld\n", trimmed_num, min_base, decimal_value);

        if (output_len >= 9900) {
            flush_buffer(output_file, output_buffer, &output_len);
        }

        free(buffer);
    }

    if (output_len > 0) {
        flush_buffer(output_file, output_buffer, &output_len);
    }

    fclose(input_file);
    fclose(output_file);
    free(output_buffer);

    return SUCCESS;
}