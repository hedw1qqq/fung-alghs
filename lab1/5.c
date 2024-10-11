#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

typedef enum {
    SUCCESS,
    ERROR_INCORRECT_ARGS,
    ERROR_INCORRECT_INPUT,
    ERROR_TAYLOR_DIVERGES,
    ERROR_EPSILON_TO_LOW
} ErrorCode;

double task1(double x, double epsilon) {
    double sum = 1.0;
    double term = 1.0;
    int n = 1;

    while (fabs(term) > epsilon) {
        term *= x / n;
        sum += term;
        n++;
    }

    return sum;
}

double task2(double x, double epsilon) {
    double sum = 1.0;
    double term = 1.0;
    int n = 1;
    while (fabs(term) > epsilon) {
        term *= -x * x / ((2 * n - 1) * (2 * n));
        sum += term;
        n++;
    }
    return sum;
}

double task3(double x, double epsilon, ErrorCode *error) {
    if (fabs(x) >= 1) {
        *error = ERROR_TAYLOR_DIVERGES;
        return 0;
    }
    int n = 0;
    double sum = 0;
    double term = 1;
    while (fabs(term) > epsilon) {
        sum += term;
        n++;
        term *= (9.0 * n * n * x * x) / ((3 * n - 1) * (3 * n - 2));
    }
    *error = SUCCESS;
    return sum;
}

double task4(double x, double epsilon, ErrorCode *error) {
    int n = 2;
    double sum = 0;
    double term = -1.0 * x * x / 2;
    if (epsilon < pow(10, -4)) {
        *error = ERROR_EPSILON_TO_LOW;
        return 0;
    }
    if (fabs(x) > 1.0) {
        *error = ERROR_TAYLOR_DIVERGES;
        return 0;
    }
    while (fabs(term) > epsilon) {
        sum += term;
        term *= -x * x * (2.0 * n - 1.0) / (2.0 * n);
        n++;
    }

    *error = SUCCESS;
    return sum;
}

int str_to_double(const char *str, double *number) {
    double result = 0.0;
    int sign = 1;
    int point_count = 0;
    double fraction = 1.0;
    int digit_count = 0;

    *number = 0.0;

    while (isspace(*str)) {
        str++;
    }

    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    while (*str != '\0') {
        if (isdigit(*str)) {
            digit_count++;
            if (point_count) {
                fraction /= 10.0;
                result += (*str - '0') * fraction;
            } else {
                result = result * 10.0 + (*str - '0');
            }
        } else if (*str == '.') {
            point_count++;
            if (point_count > 1) {
                return 0;
            }
        } else {
            return 0;
        }
        str++;
    }

    if (digit_count == 0) {
        return 0;
    }

    *number = sign * result;
    return 1;
}

ErrorCode GetOpts(int argc, char **argv, double *x, double *epsilon) {
    if (argc != 3) {
        printf("incorrect args\n");
        return ERROR_INCORRECT_ARGS;
    }
    if (!str_to_double(argv[1], epsilon)) {
        printf("Incorrect input\n");
        return ERROR_INCORRECT_INPUT;
    }
    if (!str_to_double(argv[2], x)) {
        printf("Incorrect input\n");
        return ERROR_INCORRECT_INPUT;
    }
    return SUCCESS;
}

int main(int argc, char **argv) {
    double x, epsilon;
    ErrorCode error;

    error = GetOpts(argc, argv, &x, &epsilon);
    if (error != SUCCESS) {
        return error;
    }
    if (epsilon < 0) {
        printf("invalid input");
        return ERROR_INCORRECT_INPUT;
    }
    printf("Task 1 result: %.10f\n", task1(x, epsilon));
    printf("Task 2 result: %.10f\n", task2(x, epsilon));

    double result_task3 = task3(x, epsilon, &error);
    if (error == SUCCESS) {
        printf("Task 3 result: %.10f\n", result_task3);
    } else {
        printf("TAYLOR_DIVERGES\n");
    }

    double result_task4 = task4(x, epsilon, &error);
    switch (error) {
        case SUCCESS:
            printf("Task 4 result: %.10f\n", result_task4);
            break;
        case ERROR_TAYLOR_DIVERGES:
            printf("TAYLOR_DIVERGES\n");
            break;
        case ERROR_EPSILON_TO_LOW:
            printf("EPSILON TO LOW\n");
            break;
        default:
            printf("Unknown error\n");
            break;
    }

    return SUCCESS;
}
