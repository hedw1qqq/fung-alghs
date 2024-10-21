#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

// Enum для кодов ошибок
typedef enum {
    SUCCESS = 0,
    ERROR_INVALID_BASE,
    ERROR_INVALID_INPUT,
    ERROR_BUFFER_OVERFLOW,
    ERROR_MEMORY_ALLOCATION
} ErrorCode;

// Функция для получения символа цифры
char get_digit_char(int digit, int uppercase) {
    if (digit < 10) {
        return '0' + digit;
    } else {
        return (uppercase ? 'A' : 'a') + (digit - 10);
    }
}

// Функция для конвертации числа в римскую систему счисления
ErrorCode int_to_roman(int num, char **str) {
    if (num <= 0 || num > 3999 || str == NULL) {
        return ERROR_INVALID_INPUT;
    }

    const int values[] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
    const char *symbols[] = {"M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I"};

    *str = malloc(16);  // Максимальная длина римского числа для чисел до 3999
    if (*str == NULL) {
        return ERROR_MEMORY_ALLOCATION;
    }

    char *current = *str;
    for (int i = 0; i < 13; i++) {
        while (num >= values[i]) {
            strcpy(current, symbols[i]);
            current += strlen(symbols[i]);
            num -= values[i];
        }
    }
    *current = '\0';

    return SUCCESS;
}

// Функция для конвертации числа в цекендорфово представление
ErrorCode uint_to_zeckendorf(unsigned int num, char **str) {
    if (str == NULL) {
        return ERROR_INVALID_INPUT;
    }

    unsigned int *fib = malloc(sizeof(unsigned int) * 32);
    if (fib == NULL) {
        return ERROR_MEMORY_ALLOCATION;
    }

    fib[0] = 1;
    fib[1] = 2;
    int i = 2;

    while (fib[i - 1] <= num) {
        fib[i] = fib[i - 1] + fib[i - 2];
        i++;
    }

    *str = malloc(i + 2);  // +1 для дополнительной единицы, +1 для '\0'
    if (*str == NULL) {
        free(fib);
        return ERROR_MEMORY_ALLOCATION;
    }

    char *current = *str;
    i--;
    while (i > 0) {
        if (num >= fib[i - 1]) {
            *current++ = '1';
            num -= fib[i - 1];
        } else {
            *current++ = '0';
        }
        i--;
    }
    *current++ = '1';  // Дополнительная единица в конце
    *current = '\0';

    free(fib);
    return SUCCESS;
}


// Преобразование строки из одной системы счисления в десятичную
ErrorCode str_to_int(const char *str, int base, int *result) {
    if (str == NULL || base < 2 || base > 36 || result == NULL) {
        return ERROR_INVALID_BASE;
    }

    *result = 0;
    int sign = 1;

    if (*str == '-') {
        sign = -1;
        str++;
    }

    while (*str) {
        int digit;
        if (isdigit(*str)) {
            digit = *str - '0';
        } else if (isalpha(*str)) {
            digit = tolower(*str) - 'a' + 10;
        } else {
            return ERROR_INVALID_INPUT;
        }

        if (digit >= base) {
            return ERROR_INVALID_INPUT;
        }

        // Проверка на переполнение
        if (*result > (INT_MAX - digit) / base) {
            return ERROR_BUFFER_OVERFLOW;
        }

        *result = *result * base + digit;
        str++;
    }

    *result *= sign;
    return SUCCESS;
}

// Преобразование числа в строку с заданным основанием и проверкой переполнения
ErrorCode int_to_base(int num, int base, char **str, int uppercase) {
    if (base < 2 || base > 36 || str == NULL) {
        return ERROR_INVALID_BASE;
    }

    int is_negative = 0;
    int unum;

    if (num < 0) {
        is_negative = 1;
        unum = abs(num) ;
    } else {
        unum = num;
    }

    int len = (is_negative ? 1 : 0) + 1;
    int temp = unum;
    while (temp > 0) {
        temp /= base;
        len++;
    }

    if (len > 33) {
        return ERROR_BUFFER_OVERFLOW;
    }

    *str = malloc(len);
    if (*str == NULL) {
        return ERROR_MEMORY_ALLOCATION;
    }

    char *current = *str + len - 1;
    *current-- = '\0';

    do {
        *current-- = get_digit_char(unum % base, uppercase);
        unum /= base;
    } while (unum > 0);

    if (is_negative) {
        *current = '-';
    }

    return SUCCESS;
}

// Функция для печати дампа памяти
char *memory_dump_to_string(const void *ptr, size_t size) {
    char *result = malloc(size * 9 + 1);  // 8 бит + пробел для каждого байта, +1 для '\0'
    if (result == NULL) {
        return NULL;
    }

    const unsigned char *bytes = (const unsigned char *) ptr;
    char *current = result;

    for (size_t i = 0; i < size; i++) {
        if (i > 0) {
            *current++ = ' ';
        }
        for (int j = 7; j >= 0; j--) {
            *current++ = (bytes[i] & (1 << j)) ? '1' : '0';
        }
    }
    *current = '\0';

    return result;
}

int overfprintf(FILE *stream, const char *format, ...) {
    va_list args;
    va_start(args, format);

    int chars_written = 0;
    char *buffer = NULL;
    ErrorCode err;

    while (*format) {
        if (*format == '%') {
            format++;

            if (*format == 'R' && *(format + 1) == 'o') {
                int num = va_arg(args, int);
                err = int_to_roman(num, &buffer);
                if (err == SUCCESS) {
                    chars_written += fprintf(stream, "%s", buffer);
                    free(buffer);
                } else {
                    chars_written += fprintf(stream, "[Error: %d]", err);
                }
                format += 2;
            } else if (*format == 'Z' && *(format + 1) == 'r') {
                unsigned int num = va_arg(args, unsigned int);
                err = uint_to_zeckendorf(num, &buffer);
                if (err == SUCCESS) {
                    chars_written += fprintf(stream, "%s", buffer);
                    free(buffer);
                } else {
                    chars_written += fprintf(stream, "[Error: %d]", err);
                }
                format += 2;
            } else if (*format == 'C' && (*(format + 1) == 'v' || *(format + 1) == 'V')) {
                int num = va_arg(args, int);
                int base = va_arg(args, int);
                err = int_to_base(num, base, &buffer, *(format + 1) == 'V');
                if (err == SUCCESS) {
                    chars_written += fprintf(stream, "%s", buffer);
                    free(buffer);
                } else {
                    chars_written += fprintf(stream, "[Error: %d]", err);
                }
                format += 2;
            } else if (*format == 't' && *(format + 1) == 'o') {
                const char *str = va_arg(args, const char*);
                int base = va_arg(args, int);
                int num;
                err = str_to_int(str, base, &num);
                if (err == SUCCESS) {
                    chars_written += fprintf(stream, "%d", num);
                } else {
                    chars_written += fprintf(stream, "[Error: %d]", err);
                }
                format += 2;
            } else if (*format == 'T' && *(format + 1) == 'O') {
                const char *str = va_arg(args, const char*);
                int base = va_arg(args, int);
                int num;
                err = str_to_int(str, base, &num);
                if (err == SUCCESS) {
                    chars_written += fprintf(stream, "%d", num);
                } else {
                    chars_written += fprintf(stream, "[Error: %d]", err);
                }
                format += 2;
            } else if (*format == 'm') {
                format++;
                if (*format == 'i') {
                    int num = va_arg(args, int);
                    buffer = memory_dump_to_string(&num, sizeof(int));
                    if (buffer) {
                        chars_written += fprintf(stream, "%s", buffer);
                        free(buffer);
                    } else {
                        chars_written += fprintf(stream, "[Error: Memory allocation]");
                    }
                } else if (*format == 'u') {
                    unsigned int num = va_arg(args, unsigned int);
                    buffer = memory_dump_to_string(&num, sizeof(unsigned int));
                    if (buffer) {
                        chars_written += fprintf(stream, "%s", buffer);
                        free(buffer);
                    } else {
                        chars_written += fprintf(stream, "[Error: Memory allocation]");
                    }
                } else if (*format == 'd') {
                    double num = va_arg(args, double);
                    buffer = memory_dump_to_string(&num, sizeof(double));
                    if (buffer) {
                        chars_written += fprintf(stream, "%s", buffer);
                        free(buffer);
                    } else {
                        chars_written += fprintf(stream, "[Error: Memory allocation]");
                    }
                } else if (*format == 'f') {
                    float num = (float) va_arg(args, double);  // va_arg promotes float to double
                    buffer = memory_dump_to_string(&num, sizeof(float));
                    if (buffer) {
                        chars_written += fprintf(stream, "%s", buffer);
                        free(buffer);
                    } else {
                        chars_written += fprintf(stream, "[Error: Memory allocation]");
                    }
                }
            } else {
                // Обработка стандартных флагов
                chars_written += vfprintf(stream, format - 1, args);
            }
        } else {
            fputc(*format, stream);
            chars_written++;
        }
        format++;
    }

    va_end(args);
    return chars_written;
}

int oversprintf(char *str, const char *format, ...) {
    va_list args;
    va_start(args, format);

    int chars_written = 0;
    char *current = str;
    char *buffer = NULL;
    ErrorCode err;

    while (*format) {
        if (*format == '%') {
            format++;

            if (*format == 'R' && *(format + 1) == 'o') {
                int num = va_arg(args, int);
                err = int_to_roman(num, &buffer);
                if (err == SUCCESS) {
                    chars_written += sprintf(current, "%s", buffer);
                    current += strlen(buffer);
                    free(buffer);
                } else {
                    chars_written += sprintf(current, "[Error: %d]", err);
                    current += strlen(current);
                }
                format += 2;
            } else if (*format == 'Z' && *(format + 1) == 'r') {
                unsigned int num = va_arg(args, unsigned int);
                err = uint_to_zeckendorf(num, &buffer);
                if (err == SUCCESS) {
                    chars_written += sprintf(current, "%s", buffer);
                    current += strlen(buffer);
                    free(buffer);
                } else {
                    chars_written += sprintf(current, "[Error: %d]", err);
                    current += strlen(current);
                }
                format += 2;
            } else if (*format == 'C' && (*(format + 1) == 'v' || *(format + 1) == 'V')) {
                int num = va_arg(args, int);
                int base = va_arg(args, int);
                err = int_to_base(num, base, &buffer, *(format + 1) == 'V');
                if (err == SUCCESS) {
                    chars_written += sprintf(current, "%s", buffer);
                    current += strlen(buffer);
                    free(buffer);
                } else {
                    chars_written += sprintf(current, "[Error: %d]", err);
                    current += strlen(current);
                }
                format += 2;
            } else if (*format == 't' && *(format + 1) == 'o') {
                const char *str = va_arg(args, const char*);
                int base = va_arg(args, int);
                int num;
                err = str_to_int(str, base, &num);
                if (err == SUCCESS) {
                    chars_written += sprintf(current, "%d", num);
                    current += strlen(current);
                } else {
                    chars_written += sprintf(current, "[Error: %d]", err);
                    current += strlen(current);
                }
                format += 2;
            } else if (*format == 'T' && *(format + 1) == 'O') {
                const char *str = va_arg(args, const char*);
                int base = va_arg(args, int);
                int num;
                err = str_to_int(str, base, &num);
                if (err == SUCCESS) {
                    chars_written += sprintf(current, "%d", num);
                    current += strlen(current);
                } else {
                    chars_written += sprintf(current, "[Error: %d]", err);
                    current += strlen(current);
                }
                format += 2;
            } else if (*format == 'm') {
                format++;
                if (*format == 'i') {
                    int num = va_arg(args, int);
                    buffer = memory_dump_to_string(&num, sizeof(int));
                    if (buffer) {
                        chars_written += sprintf(current, "%s", buffer);
                        current += strlen(buffer);
                        free(buffer);
                    } else {
                        chars_written += sprintf(current, "[Error: Memory allocation]");
                        current += strlen(current);
                    }
                } else if (*format == 'u') {
                    unsigned int num = va_arg(args, unsigned int);
                    buffer = memory_dump_to_string(&num, sizeof(unsigned int));
                    if (buffer) {
                        chars_written += sprintf(current, "%s", buffer);
                        current += strlen(buffer);
                        free(buffer);
                    } else {
                        chars_written += sprintf(current, "[Error: Memory allocation]");
                        current += strlen(current);
                    }
                } else if (*format == 'd') {
                    double num = va_arg(args, double);
                    buffer = memory_dump_to_string(&num, sizeof(double));
                    if (buffer) {
                        chars_written += sprintf(current, "%s", buffer);
                        current += strlen(buffer);
                        free(buffer);
                    } else {
                        chars_written += sprintf(current, "[Error: Memory allocation]");
                        current += strlen(current);
                    }
                } else if (*format == 'f') {
                    float num = (float) va_arg(args, double);  // va_arg promotes float to double
                    buffer = memory_dump_to_string(&num, sizeof(float));
                    if (buffer) {
                        chars_written += sprintf(current, "%s", buffer);
                        current += strlen(buffer);
                        free(buffer);
                    } else {
                        chars_written += sprintf(current, "[Error: Memory allocation]");
                        current += strlen(current);
                    }
                }
            } else {
                // Обработка стандартных флагов
                int len = vsprintf(current, format - 1, args);
                chars_written += len;
                current += len;
            }
        } else {
            *current++ = *format;
            chars_written++;
        }
        format++;
    }

    *current = '\0';
    va_end(args);
    return chars_written;
}

// Демонстрация работы функций
int main() {
    FILE *output_file = fopen("zhopa.txt", "w");
    printf("Демонстрация работы overfprintf:\n");
    overfprintf(output_file, "Римское число 42: %Ro %Zr \n", 42);
    overfprintf(output_file, "Цекендорфово представление 11: %Zr \n", 11);
    overfprintf(output_file, "Число 312 в системе с основанием 16 (нижний регистр): %Cv \n", 321, 16);
    overfprintf(output_file, "Число 16 в системе с основанием 16 (верхний регистр): %CV \n", 10, 16);
    overfprintf(output_file, "Перевод '1010' из двоичной в десятичную: %to \n", "10100101010", 2);
    overfprintf(output_file, "Перевод 'F' из шестнадцатеричной в десятичную: %TO \n", "F", 16);
    overfprintf(output_file, "Дамп памяти для int 42: %mi \n", 42);
    overfprintf(output_file, "Дамп памяти для unsigned int 42: %mu \n", 42u);
    overfprintf(output_file, "Дамп памяти для double 3.14: %md \n", 3.14);
    overfprintf(output_file, "Дамп памяти для float 3.14: %mf \n", 3.14f);

    printf("\nДемонстрация работы oversprintf: \n");
    char* buffer = malloc(10000);
    if (!buffer){
        return ERROR_MEMORY_ALLOCATION;
    }
    oversprintf(buffer, "Римское число 42: %Ro \n", 42);
    printf("%s", buffer);
    oversprintf(buffer, "Цекендорфово представление 11: %Zr \n", 11);
    printf("%s", buffer);
    oversprintf(buffer, "Число 255 в системе с основанием 16 (нижний регистр): %Cv \n", 255, 16);
    printf("%s", buffer);
    oversprintf(buffer, "Число 255 в системе с основанием 16 (верхний регистр): %CV \n", 255, 16);
    printf("%s", buffer);
    oversprintf(buffer, "Перевод '1010' из двоичной в десятичную: %to \n", "1010", 2);
    printf("%s", buffer);
    oversprintf(buffer, "Перевод 'FF' из шестнадцатеричной в десятичную: %TO \n", "FF", 16);
    printf("%s", buffer);
    oversprintf(buffer, "Дамп памяти для int 42: %mi \n", 42);
    printf("%s", buffer);
    oversprintf(buffer, "Дамп памяти для unsigned int 42: %mu \n", 42u);
    printf("%s", buffer);
    oversprintf(buffer, "Дамп памяти для double 3.14: %md \n", 3.14);
    printf("%s", buffer);
    oversprintf(buffer, "Дамп памяти для float 3.14: %mf \n", 3.14f);
    printf("%s", buffer);
    free(buffer);
    return 0;
}