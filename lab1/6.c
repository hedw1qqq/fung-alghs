#include <stdio.h>
#include <math.h>

double f_a(double x) {
    if (x == 0) {
        return 1.0;
    }
    return log(1 + x) / x;
}

double f_b(double x) {
    return exp(-x * x / 2);
}

double f_c(double x) {
    return log(1 / (1 - x));
}

double f_d(double x) {
    return pow(x, x);
}

double task(double (*f)(double), double a, double b, double epsilon) {
    double integral = 0.0;
    int n = 1;
    double delta_x, previous_integral;

    do {
        previous_integral = integral;
        integral = 0.0;
        delta_x = (b - a) / n;

        for (int i = 0; i < n; i++) {
            double x_i = a + (i + 0.5) * delta_x;
            integral += f(x_i);
        }

        integral *= delta_x;
        n *= 2;
    } while (fabs(integral - previous_integral) > epsilon);

    return integral;
}

int main() {
    double epsilon;
    printf("epsilon = ");
    scanf("%lf", &epsilon);
    
    double result_a = task(f_a, 0.0, 1.0, epsilon);
    double result_b = task(f_b, 0.0, 1.0, epsilon);
    double result_c = task(f_c, 0.0, 1.0, epsilon);
    double result_d = task(f_d, 0.0, 1.0, epsilon);

    printf("a: %.10f\n", result_a);
    printf("b: %.10f\n", result_b);
    printf("c: %.10f\n", result_c);
    printf("d: %.10f\n", result_d);
    
    return 0;
}
