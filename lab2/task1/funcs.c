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

//ErrorCode process_opt_n(const char *str, char **result) {
//    if (str == NULL || result == NULL) {
//        return ERR_INVALID_INPUT;
//    }
//
//    int length = str_len(str);
//    *result = (char *) malloc((length + 1) * sizeof(char));
//    if (*result == NULL) return MEMORY_ERROR;
//
//    char *temp = (char *) malloc((length + 1) * sizeof(char));
//    if (temp == NULL) {
//        free(*result);
//        return MEMORY_ERROR;
//    }
//
//    int digit_count = 0;
//    int alpha_count = 0;
//    int other_count = 0;
//
//    for (int i = 0; i < length; i++) {
//        if (is_digit(str[i])) {
//            (*result)[digit_count++] = str[i];
//        } else if (is_alpha(str[i])) {
//            temp[alpha_count++] = str[i];
//        } else {
//            temp[length - 1 - other_count++] = str[i];
//        }
//    }
//    for (int i = 0; i < alpha_count; i++) {
//        (*result)[digit_count + i] = temp[i];
//    }
//    for (int i = 0; i < other_count; i++) {
//        (*result)[digit_count + alpha_count + i] = temp[length - 1 - i];
//    }
//
//    (*result)[length] = '\0';
//    free(temp);
//    return SUCCESS;
//}
void concat_string(const char *strfrom, char **strto, int *len) {
    while (*strfrom != '\0') {
        (*strto)[*len] = *strfrom;
        (*len)++;
        strfrom++;
    }
    (*strto)[*len] = '\0';
}
ErrorCode process_opt_n(const char *str, char **result) {
    if (str == NULL || result == NULL) {
        return ERR_INVALID_INPUT;
    }

    int length = str_len(str);
    *result = (char *) malloc((length + 1) * sizeof(char));
    if (*result == NULL) return MEMORY_ERROR;

    char *temp_for_alpha = (char *) malloc((length + 1) * sizeof(char));
    if (temp_for_alpha == NULL) {
        free(*result);
        return MEMORY_ERROR;
    }

    char *temp_for_others = (char *) malloc((length + 1) * sizeof(char));
    if (temp_for_others == NULL) {
        free(temp_for_alpha);
        free(*result);
        return MEMORY_ERROR;
    }

    int alpha_idx = 0, others_idx = 0, result_idx = 0;
    for (int i = 0; i < length; i++) {
        if (is_digit(str[i])) {
            (*result)[result_idx++] = str[i];
        } else if (is_alpha(str[i])) {
            temp_for_alpha[alpha_idx++] = str[i];
        } else {
            temp_for_others[others_idx++] = str[i];
        }
    }
    temp_for_alpha[alpha_idx] = '\0';
    temp_for_others[others_idx] = '\0';

    concat_string(temp_for_alpha, result, &result_idx);
    concat_string(temp_for_others, result, &result_idx);

    (*result)[result_idx] = '\0';

    free(temp_for_alpha);
    free(temp_for_others);

    return SUCCESS;
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
    srand(seed);

    int len = 0, capacity = 1;
    (*result) = (char *) malloc(capacity * sizeof(char));
    if ((*result) == NULL) {
        return MEMORY_ERROR;
    }

    (*result)[0] = '\0';

    for (int i = 0; i < count_str; ++i) {
        int rand_num = rand() % count_str;


        if (rand_num != 0 && rand_num != 2 && rand_num < count_str) {
            int rand_str_len = str_len(argv[rand_num]);


            if (len + rand_str_len + 1 > capacity) {
                capacity = len + rand_str_len + 1;
                char *new_result = (char *) realloc(*result, capacity * sizeof(char));
                if (new_result == NULL) {
                    free(*result);
                    return MEMORY_ERROR;
                }
                *result = new_result;
            }
            
            concat_string(argv[rand_num], result, &len);
        }
    }

    return SUCCESS;
}


ErrorCode get_opts(int argc, char **argv, kOpts *opt) {
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