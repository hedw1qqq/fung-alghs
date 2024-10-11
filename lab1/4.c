#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum {
    OPT_D,
    OPT_I,
    OPT_S,
    OPT_A
} kOpts;

typedef enum {
    SUCCESS,
    ERR_INVALID_INPUT,
    ERR_TOO_FEW_ARGS,
    ERR_CONVERSION_FAIL,
    ERR_DIVISION_BY_ZERO,
    ERR_UNKNOWN_OPTION,
    ERR_SAME_FILE_NAMES
} ErrorCode;

ErrorCode GetOpts(int argc, char **argv, kOpts *option, char *input_filename, char *output_filename) {
    if (argc < 3) {
        return ERR_TOO_FEW_ARGS;
    }
    if (argc > 4) {
        return ERR_INVALID_INPUT;
    }

    const char *flag = argv[1];
    int use_custom_output = 0;

    if (strlen(flag) > 3) {
        return ERR_INVALID_INPUT;
    } else if (flag[0] == '/' || flag[0] == '-') {
        if (flag[1] == 'n') {
            use_custom_output = 1;
            switch (flag[2]) {
                case 'd':
                    *option = OPT_D;
                    break;
                case 'i':
                    *option = OPT_I;
                    break;
                case 's':
                    *option = OPT_S;
                    break;
                case 'a':
                    *option = OPT_A;
                    break;
                default:
                    return ERR_UNKNOWN_OPTION;
            }
        } else {
            switch (flag[1]) {
                case 'd':
                    *option = OPT_D;
                    break;
                case 'i':
                    *option = OPT_I;
                    break;
                case 's':
                    *option = OPT_S;
                    break;
                case 'a':
                    *option = OPT_A;
                    break;
                default:
                    return ERR_UNKNOWN_OPTION;
            }
        }
    } else {
        return ERR_INVALID_INPUT;
    }

    strcpy(input_filename, argv[2]);

    if (use_custom_output && argc == 4) {
        strcpy(output_filename, argv[3]);
    } else if (!use_custom_output) {
        sprintf(output_filename, "out_%s", input_filename);
    } else {
        return ERR_INVALID_INPUT;
    }

    return SUCCESS;
}

void remove_arabic_numbers(FILE *input, FILE *output) {
    int c;
    while ((c = fgetc(input)) != EOF) {
        if (c < '0' || c > '9') {
            fputc(c, output);
        }
    }
}

long long count_line_latin_letters(FILE *input) {
    int c;
    long long counter = 0;
    while ((c = fgetc(input)) != EOF && c != '\n') {
        c = toupper(c);
        if (c >= 'A' && c <= 'Z') {
            counter++;
        }
    }
    return counter;
}

void count_latin_letters(FILE *input, FILE *output) {
    long long counter;
    while (!feof(input)) {
        counter = count_line_latin_letters(input);
        fprintf(output, "%lld\n", counter);
    }
}

long long count_line_special_chars(FILE *input) {
    int c;
    long long counter = 0;
    while ((c = fgetc(input)) != EOF) {
        if (c == '\t' || (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') && c != '\t'))) {
            counter++;
        }
        if (c == '\n') {
            break;
        }
    }
    return counter;
}


void count_special_chars(FILE *input, FILE *output) {
    long long counter;
    while (!feof(input)) {
        counter = count_line_special_chars(input);
        fprintf(output, "%lld\n", counter);
    }
}


void replace_all_with_hex(FILE *input, FILE *output) {
    int ch;
    while ((ch = fgetc(input)) != EOF) {
        fprintf(output, "%02X", ch);
    }
}

int main(int argc, char **argv) {
    kOpts option;
    char input_filename[256];
    char output_filename[256];

    ErrorCode result = GetOpts(argc, argv, &option, input_filename, output_filename);
    if (result != SUCCESS) {
        switch (result) {
            case ERR_TOO_FEW_ARGS:
                printf("Error: Too few arguments.\n");
                break;
            case ERR_INVALID_INPUT:
                printf("Error: Invalid input.\n");
                break;
            case ERR_UNKNOWN_OPTION:
                printf("Error: Unknown option.\n");
                break;
            default:
                printf("Error: Unhandled error.\n");
                break;
        }
        return result;
    }
    if (strcmp(input_filename, output_filename) == 0) {
        printf("Error: Input and output files cannot be the same.\n");
        return ERR_SAME_FILE_NAMES;
    }

    FILE *input = fopen(input_filename, "r");
    if (input == NULL) {
        printf("Failed to open input file.\n");
        return ERR_INVALID_INPUT;
    }

    FILE *output = fopen(output_filename, "w");
    if (output == NULL) {
        printf("Failed to open output file.\n");
        fclose(input);
        return ERR_INVALID_INPUT;
    }
    switch (option) {
        case OPT_D:
            remove_arabic_numbers(input, output);
            break;
        case OPT_I:
            count_latin_letters(input, output);
            break;
        case OPT_S:
            count_special_chars(input, output);
            break;
        case OPT_A:
            replace_all_with_hex(input, output);
            break;
    }

    fclose(input);
    fclose(output);

    return SUCCESS;
}
