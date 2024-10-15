#include "funcs.h"


int main() {
    if (is_convex_polygon(4, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0)) {
        printf("polygon is convex\n");
    } else {
        printf("polygon is not convex.\n");
    }

    double x = 2.0;
    double result;
    Error_codes status = calculate_polynomial(&result, x, 2, 3.0, -5.0, 2.0); // Полином 3x^2 - 5x + 2
    if (status == OK){
        printf("calculate_polynomial at %.10f point %.10f\n",x,result);
    }
        find_kaprekar_numbers(10, 3, "9", "45", "55");  // Примеры для десятичной системы
        find_kaprekar_numbers(16, 3, "A", "B",'C');         // Примеры для шестнадцатеричной системы
    return 0;
}