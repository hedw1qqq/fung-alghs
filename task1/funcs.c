#include "funcs.h"

int is_digit(int c) {
    return c >= '0' && c <= '9';
}

int is_alpha(int c) {
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

int str_len(const char *str) {
    int len = 0;
    while (*str != '\0') {
        len++;
        str++;
    }
    return len;
}

ErrorCode process_opt_r(const char *str, char **result) {
    if (str == NULL || result == NULL) {
        return ERR_INVALID_INPUT;
    }

    int length = str_len(str);
    *result = (char *) malloc((length + 1) * sizeof(char));
    if (*result == NULL) {
        return MEMORY_ERROR;
    }

    for (int i = 0; i < length; i++) {
        (*result)[i] = str[length - 1 - i];
    }
    (*result)[length] = '\0';

    return SUCCESS;
}

ErrorCode process_opt_u(const char *str, char **result) {
    if (str == NULL || result == NULL) {
        return ERR_INVALID_INPUT;
    }
    int length = str_len(str);
    *result = (char *) malloc((length + 1) * sizeof(char));
    if (*result == NULL) {
        return MEMORY_ERROR;
    }
    for (int i = 0; i < length; i++) {
        if (i % 2 != 0 && is_alpha(str[i])) {
            if (str[i] >= 'a') {
                (*result)[i] = str[i] - 'a' + 'A';
                continue;
            }
        }
        (*result)[i] = str[i];
    }
    (*result)[length] = '\0';
    return SUCCESS;
}

ErrorCode process_opt_n(const char *str, char **result) {
    if (str == NULL || result == NULL) {
        return ERR_INVALID_INPUT;
    }
    int length = str_len(str);
    *result = (char *) malloc((length + 1) * sizeof(char));
    if (*result == NULL) return MEMORY_ERROR;

    int index = 0;

    for (int i = 0; i < length; i++) {
        if (is_digit(str[i])) {
            (*result)[index++] = str[i];
        }
    }

    for (int i = 0; i < length; i++) {
        if (is_alpha(str[i])) {
            (*result)[index++] = str[i];
        }
    }

    for (int i = 0; i < length; i++) {
        if (!is_alpha(str[i]) && !is_digit(str[i])) {
            (*result)[index++] = str[i];
        }
    }

    (*result)[index] = '\0';
    return SUCCESS;
}

void concat_string(char *str, char **result, int *len) {
    for (int i = 0; str[i] != '\0'; ++i) {
        (*result)[(*len)++] = str[i];
    }
    (*result)[(*len)] = '\0';
}

ErrorCode str_to_int(const char *number_str, int *number) {
    if (number_str == NULL || number == NULL) {
        return ERR_INVALID_INPUT;
    }

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
                return ERR_OVERFLOW;
            }
            *number = *number * 10 + (number_str[j] - '0');
        } else {
            return ERR_INVALID_INPUT;
        }
    }

    if (is_negative) {
        *number = -*number;
        if (*number < INT_MIN) {
            return ERR_OVERFLOW;
        }
    }

    return SUCCESS;
}

ErrorCode process_opt_c(char **argv, int count_str, char **result, int seed) {
    int len = 0, capacity = 0;
    for (int i = 0; i < count_str; ++i) {
        len = str_len(argv[i]);
        if (len > capacity) capacity = len;
    }
    capacity *= count_str;
    len = 0;
    (*result) = (char *) malloc(sizeof(char) * (capacity + 1));
    if ((*result) == NULL) {
        return MEMORY_ERROR;
    }
    srand(seed);
    int rand_num;
    for (int i = 0; i < count_str; ++i) {
        rand_num = rand() % count_str;
        if (rand_num != 0 && rand_num != 2) {
            concat_string(argv[rand_num], result, &len);
        }
    }
    return SUCCESS;
}


ErrorCode GetOpts(int argc, char **argv, kOpts *opt) {
    if (argc < 3 || argv == NULL || opt == NULL) {
        return ERR_INVALID_ARGS;
    }

    const char *flag = argv[1];
    if (flag[0] == '-' && str_len(flag) == 2) {
        switch (flag[1]) {
            case 'l':
                if (argc != 3) return ERR_INVALID_ARGS;
                *opt = opt_l;
                return SUCCESS;
            case 'r':
                if (argc != 3) return ERR_INVALID_ARGS;
                *opt = opt_r;
                return SUCCESS;
            case 'u':
                if (argc != 3) return ERR_INVALID_ARGS;
                *opt = opt_u;
                return SUCCESS;
            case 'n':
                if (argc != 3) return ERR_INVALID_ARGS;
                *opt = opt_n;
                return SUCCESS;
            case 'c':
                if (argc < 5) return ERR_INVALID_ARGS;
                *opt = opt_c;
                return SUCCESS;
            default:
                return ERR_INVALID_ARGS;
        }
    }
    return ERR_INVALID_ARGS;
}