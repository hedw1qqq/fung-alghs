#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef enum kOpts {
    OPT_Q, OPT_M, OPT_T
} kOpts;

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
    } else {
        if (*number > INT_MAX) {
            return 0;
        }
    }

    return 1;
}


int compare_double(double a, double b, double epsilon) {
    return fabs(a - b) < epsilon;
}

char *solve_linear(double b, double c, double epsilon) {
    static char result[100];

    if (compare_double(b, 0.0, epsilon)) {
        if (compare_double(c, 0.0, epsilon)) {
            snprintf(result, sizeof(result), "Infinite solutions (0 = 0).");
        } else {
            snprintf(result, sizeof(result), "No solutions.");
        }
    } else {
        double root = -c / b;
        snprintf(result, sizeof(result), "One root: %.6f", root);
    }

    return result;
}

char *solve_permutations(double epsilon, double a, double b, double c) {
    static char result[200];

    if (compare_double(a, 0.0, epsilon)) {
        return solve_linear(b, c, epsilon);
    } else {
        double discriminant = b * b - 4 * a * c;
        if (compare_double(discriminant, 0.0, epsilon)) {
            double root = -b / (2 * a);
            snprintf(result, sizeof(result), "One real root: %.6f", root);
        } else if (discriminant > 0) {
            double root1 = (-b + sqrt(discriminant)) / (2 * a);
            double root2 = (-b - sqrt(discriminant)) / (2 * a);
            snprintf(result, sizeof(result), "Two real roots: %.6f and %.6f", root1, root2);
        } else {
            snprintf(result, sizeof(result), "No real roots.");
        }
    }

    return result;
}

void HandlerOptQ(double epsilon, double a, double b, double c) {
    printf("Solving for all permutations of coefficients:\n");

    printf("solves for a = %.6f, b = %.6f, c = %.6f\n", a, b, c);
    printf("%s\n", solve_permutations(epsilon, a, b, c));

    printf("solves for a = %.6f, b = %.6f, c = %.6f\n", a, c, b);
    printf("%s\n", solve_permutations(epsilon, a, c, b));

    printf("solves for a = %.6f, b = %.6f, c = %.6f\n", b, a, c);
    printf("%s\n", solve_permutations(epsilon, b, a, c));

    printf("solves for a = %.6f, b = %.6f, c = %.6f\n", b, c, a);
    printf("%s\n", solve_permutations(epsilon, b, c, a));

    printf("solves for a = %.6f, b = %.6f, c = %.6f\n", c, a, b);
    printf("%s\n", solve_permutations(epsilon, c, a, b));

    printf("solves for a = %.6f, b = %.6f, c = %.6f\n", c, b, a);
    printf("%s\n", solve_permutations(epsilon, c, b, a));
}

void HandlerOptM(int x, int y) {
    if (y == 0) {
        printf("Division by zero error\n");
        return;
    }

    if (x % y == 0) {
        printf("%d is divisible by %d\n", x, y);
    } else {
        printf("%d is not divisible by %d\n", x, y);
    }
}

int is_right_triangle(double a, double b, double c, double epsilon) {
    double max_side = fmax(a, fmax(b, c));
    double sum_of_squares = a * a + b * b + c * c - max_side * max_side;

    return compare_double(max_side * max_side, sum_of_squares, epsilon);
}

void HandlerOptT(double epsilon, double a, double b, double c) {
    if (is_right_triangle(a, b, c, epsilon)) {
        printf("The sides form a right triangle\n");
    } else {
        printf("The sides do not form a right triangle\n");
    }
}

int GetOpts(int argc, char **argv, kOpts *option, double *numbers) {
    if (argc < 3) {
        return 1;
    }

    const char *proceeding_option = argv[1];
    if (strlen(proceeding_option) != 2) {
        return 1;
    } else if (proceeding_option[0] == '/' || proceeding_option[0] == '-') {
        switch (proceeding_option[1]) {
            case 'q':
                if (argc != 6) return 1;
                *option = OPT_Q;
                for (int i = 0; i < 4; i++) {
                    if (!str_to_double(argv[2 + i], &numbers[i])) {
                        printf("Invalid number format");
                        return 1;
                    }
                }
                break;
            case 'm':
                if (argc != 4) return 1;
                *option = OPT_M;
                int x, y;
                if (str_to_int(argv[2], &x) && str_to_int(argv[3], &y)) {
                    numbers[0] = x;
                    numbers[1] = y;
                } else {
                    printf("Invalid number format");
                    return 1;
                }
                break;
            case 't':
                if (argc != 6) return 1;
                *option = OPT_T;
                for (int i = 0; i < 4; i++) {
                    if (!str_to_double(argv[2 + i], &numbers[i])) {
                        printf("Invalid number format");
                        return 1;
                    }
                }
                break;
            default:
                return 1;
        }
    } else {
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    kOpts opt = 0;
    double numbers[4] = {0};

    if (GetOpts(argc, argv, &opt, numbers)) {
        printf("Incorrect option or parameters\n");
        return 1;
    }

    switch (opt) {
        case OPT_Q:
            HandlerOptQ(numbers[0], numbers[1], numbers[2], numbers[3]);
            break;
        case OPT_M:
            HandlerOptM((int) numbers[0], (int) numbers[1]);
            break;
        case OPT_T:
            HandlerOptT(numbers[0], numbers[1], numbers[2], numbers[3]);
            break;
        default:
            printf("Unknown option\n");
            return 1;
    }

    return 0;
}