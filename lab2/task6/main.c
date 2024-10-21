#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <limits.h>

// Прототипы вспомогательных функций
int parse_roman(const char *str, int *result);
int parse_zr(const char *str, unsigned int *result);
int parse_base(const char *str, int *result, int base, int is_uppercase);

// Функция overfscanf (обработка файла)
int overfscanf(FILE *stream, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int count = 0;
    const char *ptr = format;

    while (*ptr != '\0') {
        if (*ptr == '%') {
            ptr++;
            if (*ptr == 'R' && *(ptr + 1) == 'o') {
                ptr += 2; // Пропускаем Ro
                int *dest = va_arg(args, int *);
                char buffer[100];
                if (fscanf(stream, "%99s", buffer) == 1 && parse_roman(buffer, dest)) {
                    count++;
                }
            } else if (*ptr == 'Z' && *(ptr + 1) == 'r') {
                ptr += 2; // Пропускаем Zr
                unsigned int *dest = va_arg(args, unsigned int *);
                char buffer[100];
                if (fscanf(stream, "%99s", buffer) == 1 && parse_zr(buffer, dest)) {
                    count++;
                }
            } else if (*ptr == 'C' && (*(ptr + 1) == 'v' || *(ptr + 1) == 'V')) {
                int is_uppercase = (*(ptr + 1) == 'V');
                ptr += 2; // Пропускаем Cv или CV
                int *dest = va_arg(args, int *);
                int base = va_arg(args, int);
                char buffer[100];
                if (fscanf(stream, "%99s", buffer) == 1 && parse_base(buffer, dest, base, is_uppercase)) {
                    count++;
                }
            } else {
                // Обработка стандартных форматов
                char format_buffer[3] = {'%', *ptr, '\0'};
                void *dest = va_arg(args, void *);
                if (vfscanf(stream, format_buffer, args) == 1) {
                    count++;
                }
            }
        } else {
            ptr++;
        }
    }

    va_end(args);
    return count;
}

// Функция для римских цифр
int parse_roman(const char *str, int *result) {
    int values[256] = {0};
    values['I'] = 1;
    values['V'] = 5;
    values['X'] = 10;
    values['L'] = 50;
    values['C'] = 100;
    values['D'] = 500;
    values['M'] = 1000;

    int res = 0, prev_value = 0;

    for (int i = strlen(str) - 1; i >= 0; i--) {
        int current_value = values[(unsigned char)str[i]];
        if (current_value == 0) return 0;  // Некорректный символ

        if (current_value < prev_value) {
            res -= current_value;
        } else {
            res += current_value;
        }
        prev_value = current_value;
    }

    *result = res;
    return 1;
}

// Функция для Цекендорфова представления
int parse_zr(const char *str, unsigned int *result) {
    unsigned int fib[100] = {1, 2}; // Массив чисел Фибоначчи
    int fib_count = 2;
    unsigned int res = 0;
    int len = strlen(str);

    // Генерируем числа Фибоначчи
    while (fib[fib_count - 1] < UINT_MAX / 2) {
        fib[fib_count] = fib[fib_count - 1] + fib[fib_count - 2];
        fib_count++;
    }

    // Проверяем, что строка заканчивается на '1'
    if (str[len - 1] != '1') return 0;

    for (int i = 0, j = fib_count - 1; i < len - 1 && j >= 0; i++, j--) {
        if (str[i] == '1') {
            res += fib[j];
        } else if (str[i] != '0') {
            return 0; // Некорректный символ
        }
    }

    *result = res;
    return 1;
}

// Функция для системы счисления
int parse_base(const char *str, int *result, int base, int is_uppercase) {
    if (base < 2 || base > 36) {
        base = 10;
    }

    char *endptr;
    long value = strtol(str, &endptr, base);

    if (*endptr == '\0' && value >= INT_MIN && value <= INT_MAX) {
        // Проверка регистра
        for (const char *s = str; *s; s++) {
            if (isalpha((unsigned char)*s)) {
                if ((is_uppercase && islower((unsigned char)*s)) ||
                    (!is_uppercase && isupper((unsigned char)*s))) {
                    return 0;
                }
            }
        }
        *result = (int)value;
        return 1;
    }
    return 0;
}

// Пример использования
int main() {
    FILE *file = fopen("123.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    int roman_value;
    unsigned int zr_value;
    int base_value;

    // Пример использования функции overfscanf
    int count = overfscanf(file, "%Ro %Zr %Cv", &roman_value, &zr_value, &base_value, 36);
    printf("countt: %d\n", count);
    printf("Roman: %d, Zr: %u, Base: %d\n", roman_value, zr_value, base_value);

    fclose(file);
    return 0;
}