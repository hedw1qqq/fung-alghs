//
// Created by ivglu on 13.10.2024.
//

#include "funcs.h"


double cross_product(Point a, Point b, Point c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

Error_codes is_convex_polygon(int num_points, ...) {
    if (num_points < 3) return ERROR_INVALID_INPUT;

    va_list args;
    va_start(args, num_points);

    Point points[num_points];

    for (int i = 0; i < num_points; i++) {
        points[i].x = va_arg(args, double);
        points[i].y = va_arg(args, double);
    }

    va_end(args);

    int sign = 0;

    for (int i = 0; i < num_points; i++) {
        double cross = cross_product(points[i], points[(i + 1) % num_points], points[(i + 2) % num_points]);
        if (cross != 0) {
            if (sign == 0) {
                sign = (cross > 0) ? 1 : -1;
            } else if ((cross > 0 && sign == -1) || (cross < 0 && sign == 1)) {
                return 0;
            }
        }
    }

    return OK;
}

Error_codes calculate_polynomial(double * result, double x, int n, ...)
{
    va_list args;
    va_start(args, n);

    *result = 0;
    for(int i = n; i >= 0; --i)
    {
        double coeff = va_arg(args, double);
        *result += coeff * pow(x, i);
        if(isinf(*result) || isnan(*result))
        {
            return OVERFLOW_ERROR;
        }
    }

    va_end(args);
    return OK;
}

int is_kaprekar(int number, int base) {
    long long square = (long long)number * number;
    int digits = 0;
    long long temp = square;

    // Подсчет количества цифр в числе при данном основании системы счисления
    while (temp > 0) {
        digits++;
        temp /= base;
    }

    // Пробуем все возможные разделения числа
    for (int i = 1; i < digits; i++) {
        long long divisor = 1;
        for (int j = 0; j < i; j++) {
            divisor *= base; // безопасное возведение в степень с целыми числами
        }
        long long right = square % divisor;
        long long left = square / divisor;

        // Проверяем, чтобы правая часть была положительной и сумма частей была равна исходному числу
        if (right > 0 && left + right == number) {
            return 1; // Это число Капрекара
        }
    }

    return 0; // Не является числом Капрекара
}

int str_to_int_base(const char *str, int base) {
    return (int)strtol(str, NULL, base);
}

void find_kaprekar_numbers(int base, int num_args, ...) {
    va_list args;
    va_start(args, num_args);

    printf("Kaprekar numbers in base %d:\n", base);

    for (int i = 0; i < num_args; i++) {
        const char *number_str = va_arg(args, const char*);
        int number = str_to_int_base(number_str, base);

        if (is_kaprekar(number, base)) {
            printf("%s (base %d) is a Kaprekar number\n", number_str, base);
        } else {
            printf("%s (base %d) is not a Kaprekar number\n", number_str, base);
        }
    }

    va_end(args);
}