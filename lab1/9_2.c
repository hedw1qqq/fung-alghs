#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

float findClosestElement(float *B, float x, int size) {
    if (size == 0) {
        printf("Vector B is empty\n");
        return 1;
    }
    float closest = B[0];
    float minDiff = fabsf(x - B[0]);

    for (int i = 1; i < size; i++) {
        float diff = fabsf(x - B[i]);
        if (diff < minDiff) {
            minDiff = diff;
            closest = B[i];
        }
    }
    return closest;
}

void generateRandomVector(float *arr, int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = -1000 + rand() % 2001;
    }
}

void printVector(float *arr, int size) {
    if (size == 0) {
        printf("Vector is empty\n");
        return;
    }
    for (int i = 0; i < size; i++) {
        printf("%.2f ", arr[i]);
    }
    printf("\n\n\n");
}

float *swapClosestElement(float *A, float *B, int size) {
    float *C = malloc(sizeof(float) * size);
    if (C == NULL) {
        printf("Memory allocation error for array C\n");
        return NULL;
    }
    for (int i = 0; i < size; i++) {
        C[i] = A[i] + findClosestElement(B, A[i], size);
    }
    return C;
}

int main() {
    srand(time(NULL));
    int size = 10;
    float *A = malloc(sizeof(float) * size);
    float *B = malloc(sizeof(float) * size);
    if (A == NULL || B == NULL) {
        printf("Memory allocation error for arrays A or B\n");
        free(A);
        free(B);
        return 1;
    }

    generateRandomVector(A, size);
    generateRandomVector(B, size);

    printVector(A, size);
    printVector(B, size);

    float *C = swapClosestElement(A, B, size);

    if (C != NULL) {
        printVector(C, size);
        free(C);
    }

    free(A);
    free(B);

    return 0;
}
