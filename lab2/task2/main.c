#include "funcs.h"


int main() {
    double result;
    ErrorCode code = geometric_mean(&result, 4, 3.0, 2.0, 1.0, 100.0);
    switch (code) {

        case SUCCESS:
            printf("%.10f\n", result);

            break;

        case INVALID_INPUT:
            printf("INVALID INPUT\n");
            break;
        case OVERFLOW_ERROR:
            printf("OVERFLOW ERROR\n");
            break;
        default:
            printf("Unknown Error\n");
            break;
    }
    double result_pow;
    double base_pow = 4;
    int exponent = -2;
    ErrorCode status = fast_pow(base_pow, exponent, &result_pow);
    switch (status) {

        case SUCCESS:
            printf("%.10f", result_pow);
            break;
        case INVALID_INPUT:
            printf("INVALID INPUT");
            break;
        case OVERFLOW_ERROR:
            printf("OVERFLOW ERROR");
            break;
        default:
            printf("Unknown Error");
            break;
    }
    return 0;
}
