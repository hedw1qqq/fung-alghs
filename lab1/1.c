#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
typedef enum kOpts {
    OPT_H, OPT_P, OPT_S, OPT_E, OPT_A, OPT_F
} kOpts;

int GetOpts(int argc, char **argv, kOpts *option, int *number) {
    if (argc != 3) {
        return 1;
    }
    const char *procceding_option = argv[1];
    if (procceding_option[0] == '/' || procceding_option[0] == '-') {
        switch (procceding_option[1]) {
            case 'h':
                *option = OPT_H;
                break;
            case 'p':
                *option = OPT_P;
                break;
            case 's':
                *option = OPT_S;
                break;
            case 'e':
                *option = OPT_E;
                break;
            case 'a':
                *option = OPT_A;
                break;
            case 'f':
                *option = OPT_F;
                break;
            default:
                return 1;
        }
    } else {
        return 1;
    }
    const char *number_str = argv[2];
    int is_negative = 0;
    int j = 0;

    if (number_str[0] == '-') {
        is_negative = 1;
        j = 1;
    }
    *number = 0;
    for (; number_str[j]; ++j) {
        if (number_str[j] >= '0' && number_str[j] <= '9') {
            *number *= 10;
            *number += number_str[j] - '0';
        } else {
            return 1;
        }
    }

    if (is_negative) {
        *number = -*number;
    }

    return 0;
}

void HandlerOptH(int x) {
    if (x == 0) {
        printf("Division by zero");
        return;
    }
    int tmp = abs(x);
    int flag = 0;
    for (int i = tmp; i <= 100; i += tmp) {
        printf("%d ", i);
        flag = 1;
    }
    if (!flag) {
        printf("Not found numbers multiple %d", x);
    }
}

void HandlerOptP(int x) {
    int flag = 1;
    
    for (int i = 2; i <= sqrt(x); ++i) {
        if (!(abs(x) % i)) {
            flag = 0;
            break;
        }
    }

    if (flag && abs(x)!=1) {
        printf("%d is a simple\n", x);
    } else {
        printf("%d is not a simple\n", x);
    }
}

void HandlerOptS(int x) {
    char hex[9];
    int i = 0;

    if (x == 0) {
        printf("0\n");
        return;
    }

    while (x > 0) {
        int digit = x % 16;
        if (digit < 10) {
            hex[i] = '0' + digit;
        } else {
            hex[i] = 'A' + (digit - 10);
        }
        x /= 16;
        i++;
    }

    for (int j = i - 1; j >= 0; j--) {
        printf("%c ", hex[j]);
    }
    return;
}

void HandlerOptE(int x) {
    if (x <= 10 && x >=1) {
        printf("x must be 1 <= x <= 10");
        return;
    }
    for (int i = 1; i <= 10; i++) {
        printf("%d^%d = %d \n", i, x, (int) pow(i, x));
    }
    return;
}

void HandlerOptA(int x) {
    if (x <1){
        printf("x must be >=1");
        return;
    }
    int sum = x * (1 + x) / 2;
    printf("sum = %d", sum);
    return;
}

void HandlerOptF(int x) {
    int s = 1;
    for (int i = 1; i <= x; i++) {
        if (s > INT_MAX || s <=0) {
            printf("overflow", x);
            return;
        }
        s *= i;
    }
    printf("x! = %d\n", s);
    return;
}

int main(int argc, char **argv) {
    kOpts opt = 0;
    int procceed_number = 0;
    void (*handlers[6])(int) = {HandlerOptH, HandlerOptP, HandlerOptS,
                                HandlerOptE, HandlerOptA, HandlerOptF};

    if (GetOpts(argc, argv, &opt, &procceed_number)) {
        printf("%s", "Incorrect option");
        return 1;
    }
    handlers[opt](procceed_number);
    return 0;
}