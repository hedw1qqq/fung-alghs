//
// Created by ivglu on 26.10.2024.
//

#ifndef LAB3_FUNCS_H
#define LAB3_FUNCS_H

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    OK,
    INVALID_INPUT,
    INVALID_MEMORY,
} errors;

int sum(int num1, int num2);

int negative(int n);

int sum_with_negative(int a, int b);

void reverse_string(char *str, int length);

int is_power_of_two(int n);

errors decimal_into_powern(int num, int base, char **res, int *capacity, int *count);

#endif //LAB3_FUNCS_H