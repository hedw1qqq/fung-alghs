#ifndef TASK1_FUNCS_H
#define TASK1_FUNCS_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef enum {
    opt_l,
    opt_r,
    opt_u,
    opt_n,
    opt_c
} kOpts;

typedef enum {
    SUCCESS,
    ERR_INVALID_INPUT,
    MEMORY_ERROR,
    ERR_OVERFLOW,
    ERR_INVALID_ARGS
} ErrorCode;

int str_len(const char *str);
ErrorCode str_to_int(const char *number_str, int *number);
ErrorCode process_opt_r(const char *str, char **result);
ErrorCode process_opt_u(const char *str, char **result);
ErrorCode process_opt_n(const char *str, char **result);
ErrorCode process_opt_c(char **argv, int count_str, char **result, int seed);
ErrorCode get_opts(int argc, char **argv, kOpts *opt);

#endif //TASK1_FUNCS_H