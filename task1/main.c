#include "funcs.h"

int main(int argc, char *argv[]) {
    kOpts opt;
    ErrorCode code;

    code = GetOpts(argc, argv, &opt);
    if (code != SUCCESS) {
        printf("Error: Invalid arguments\n");
        return code;
    }

    switch (opt) {
        case opt_l: {
            int length = str_len(argv[2]);
            printf("Length: %d\n", length);
            break;
        }
        case opt_r: {
            char *result;
            code = process_opt_r(argv[2], &result);
            if (code == SUCCESS) {
                printf("result: %s\n", result);
                free(result);
            } else {
                printf("Error processing option -r\n");
            }
            break;
        }
        case opt_u: {
            char *result;
            code = process_opt_u(argv[2], &result);
            if (code == SUCCESS) {
                printf("result: %s\n", result);
                free(result);
            } else {
                printf("Error processing option -u\n");
            }
            break;
        }
        case opt_n: {
            char *result;
            code = process_opt_n(argv[2], &result);
            if (code == SUCCESS) {
                printf("%s\n", result);
                free(result);
            } else {
                printf("Error processing option -n\n");
            }
            break;
        }
        case opt_c: {
            int count_str = argc - 3;
            if (count_str < 2) {
                printf("Invalid input\n");
                return ERR_INVALID_INPUT;
            }
            int seed;
            if (str_to_int(argv[2], &seed) != SUCCESS) {
                printf("Invalid input\n");
                return ERR_INVALID_INPUT;
            }
            char *result_c;
            code = process_opt_c(argv + 3, count_str, &result_c, seed);
            if (code == SUCCESS) {
                printf("%s\n", result_c);
                free(result_c);
            } else {
                printf("Error processing option -c\n");
            }
            break;
        }
        default:
            printf("Error: Unknown operation\n");
            return ERR_INVALID_ARGS;
    }

    return SUCCESS;
}