#include <stdio.h>
#include <math.h>

typedef enum {
    INVALID_INPUT,
    NO_ROOT,
    SUCCESS
} error_code;

error_code dichotomy(double *result, double a, double b, double epsilon, double (*f)(double)) {
    *result = 0;

    if (f(a) * f(b) >= 0) {
        return NO_ROOT;
    }

    if (epsilon <= 0) {
        return INVALID_INPUT;
    }

    while ((b - a) >= epsilon) {
        *result = (a + b) / 2;
        if (f(*result) * f(a) < 0)
            b = *result;
        else
            a = *result;
    }

    return SUCCESS;
}


double dichotomy_ln2(double x) {
    return exp(x) - 2;
}


double dichotomy_sqrt2(double x) {
    return pow(x, 2) - 2;
}

double dichotomy_e(double x) {
    return log(x) - 1;
}


double dichotomy_linear(double x) {
    return x - 5;
}

double dichotomy_quadratic(double x) {
    return x * x - 4 * x + 3;
}

double dichotomy_cubic(double x) {
    return x * x * x - 3 * x * x + 2;
}

double dichotomy_trigonometric(double x) {
    return sin(x) - 0.5;
}

void handle_result(error_code code, double result, const char *name) {
    if (code == SUCCESS) {
        printf("for %s: %.10f\n", name, result);
    } else if (code == NO_ROOT) {
        printf("for %s: no root on given interval\n", name);
    } else if (code == INVALID_INPUT) {
        printf("for %s: invalid input .\n", name);
    }
}

int main() {
    double result;
    error_code code;

    code = dichotomy(&result, -4, -1, 0.000001, dichotomy_ln2);
    handle_result(code, result, "ln2");

    code = dichotomy(&result, 1, 2, 0.000001, dichotomy_sqrt2);
    handle_result(code, result, "sqrt(2)");

    code = dichotomy(&result, 1, 3, 0.000001, dichotomy_e);
    handle_result(code, result, "e");

    code = dichotomy(&result, -5, 10, 0.000001, dichotomy_linear);
    handle_result(code, result, "linear");

    // (x = 1)
    code = dichotomy(&result, -23, 2, 0.000001, dichotomy_quadratic);
    handle_result(code, result, "quadratic (root 1)");

    // (x = 3)
    code = dichotomy(&result, 2, 44, 0.000001, dichotomy_quadratic);
    handle_result(code, result, "quadratic (root 2)");

    code = dichotomy(&result, 0, 3, 0.000001, dichotomy_cubic);
    handle_result(code, result, "cubic");

    code = dichotomy(&result, 0, 2, 0.000001, dichotomy_trigonometric);
    handle_result(code, result, "sinx = 0.5");

    return 0;
}

