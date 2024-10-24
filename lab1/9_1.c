#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

int str_to_int(char *number_str, int *number) {
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
                return 0;
            }
            *number *= 10;
            *number += number_str[j] - '0';
        } else {
            return 0;
        }
    }
    if (is_negative) {
        *number = -*number;
        if (*number < INT_MIN) {
            return 0;
        }
    }

    return 1;
}

void generateArray(float *array, int size, int a, int b) {
    for (int i = 0; i < size; i++) {
        array[i] = a + (float) (rand() % b);
    }
}

void printArray(float *array, int size) {
    for (int i = 0; i < size; i++) {
        printf("%.2f ", array[i]);
    }
    printf("\n");
}

void findMinMax(float *array, int size, int *indmin, int *indmax) {
    float minx = array[0];
    float maxx = array[0];
    *indmin = 0;
    *indmax = 0;

    for (int i = 1; i < size; i++) {
        if (array[i] > maxx) {
            maxx = array[i];
            *indmax = i;
        }
        if (array[i] < minx) {
            minx = array[i];
            *indmin = i;
        }
    }
}

void swap(float *array, int indmin, int indmax) {
    float temp = array[indmin];
    array[indmin] = array[indmax];
    array[indmax] = temp;
}

int main() {
    srand(time(NULL));

    int a, b;
    char s1[100], s2[100];

    printf("a = ");
    scanf("%s", s1);
    if (!str_to_int(s1, &a)) {
        printf("Incorrect input for a\n");
        return 1;
    }

    printf("b = ");
    scanf("%s", s2);
    if (!str_to_int(s2, &b)) {
        printf("Incorrect input for b\n");
        return 1;
    }

    if (a > b) {
        printf("a must be <= b\n");
        return 1;
    }
    int size = 10;

    float array[size];

    generateArray(array, size, a, b);
    printf("Original array: ");
    printArray(array, size);

    int indmin, indmax;
    findMinMax(array, size, &indmin, &indmax);

    swap(array, indmin, indmax);

    printf("Modified array: ");
    printArray(array, size);

    return 0;
}
