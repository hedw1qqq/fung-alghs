#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


typedef enum {
    OPT_R,
    OPT_A
} kOpts;

typedef enum {
    SUCCESS,
    ERR_INVALID_ARGUMENTS,
    ERR_SAME_FILE_NAMES,
    ERR_FILE_FAIL,
    ERR_OPTION_FAIL,
    ERR_MEMORY_ERROR
} ErrorCode;


void flush_buffer(FILE *output, char *buffer, size_t *output_len) {
    fprintf(output, "%s", buffer);
    *output_len = 0;
    buffer[0] = '\0';
}

int GetOpts(int argc, char **argv, kOpts *option) {
    if (argc < 4 || argc > 5) {
        return ERR_INVALID_ARGUMENTS;
    }

    const char *proceeding_option = argv[1];

    if (strlen(proceeding_option) != 2 || (proceeding_option[0] != '/' && proceeding_option[0] != '-')) {
        return ERR_OPTION_FAIL;
    }

    if (proceeding_option[1] == 'r' && argc == 5) {
        *option = OPT_R;
    } else if (proceeding_option[1] == 'a' && argc == 4) {
        *option = OPT_A;
    } else {
        return ERR_OPTION_FAIL;
    }

    return SUCCESS;
}

char *read_lexeme(FILE *file) {
    size_t size = 32;
    size_t len = 0;
    char *buffer = malloc(size);

    if (!buffer) {
        return NULL;
    }

    int c;
    while ((c = fgetc(file)) != EOF && isspace(c));

    if (c == EOF) {
        free(buffer);
        return NULL;
    }

    do {
        buffer[len++] = c;

        if (len == size) {
            size *= 2;
            char *new_buffer = realloc(buffer, size);
            if (!new_buffer) {
                free(buffer);
                return NULL;
            }
            buffer = new_buffer;
        }

        c = fgetc(file);
    } while (c != EOF && !isspace(c));

    buffer[len] = '\0';

    if (len == 0) {
        free(buffer);
        return NULL;
    }

    return buffer;
}

int check_file_names(const char *file1, const char *file2) {
    if (strcmp(file1, file2) == 0) {
        return ERR_SAME_FILE_NAMES;
    }
    return SUCCESS;
}

ErrorCode process_r_option(FILE *file1, FILE *file2, FILE *output) {
    char *buffer1 = NULL;
    char *buffer2 = NULL;
    char *output_buffer = malloc(10000);
    if (!output_buffer) {
        return ERR_MEMORY_ERROR;
    }
    size_t output_len = 0;
    int turn = 1;

    while (1) {
        if (turn % 2 != 0) {
            buffer1 = read_lexeme(file1);
            if (buffer1) {
                output_len += sprintf(output_buffer + output_len, "%s ", buffer1);
                free(buffer1);
            }
        } else {
            buffer2 = read_lexeme(file2);
            if (buffer2) {
                output_len += sprintf(output_buffer + output_len, "%s ", buffer2);
                free(buffer2);
            }
        }
        if (output_len >= 10000 - 100) {
            flush_buffer(output, output_buffer, &output_len);
        }
        if (!buffer1 && !buffer2) {
            break;
        }

        turn++;
    }

    if (output_len > 0) {
        flush_buffer(output, output_buffer, &output_len);
    }
    free(output_buffer);
    return SUCCESS;
}

char *char_to_base4(char c) {
    int ascii = (int) c;
    static char result[5];
    int i = 3;

    while (i >= 0) {
        result[i--] = (ascii % 4) + '0';
        ascii /= 4;
    }
    result[4] = '\0';

    return result;
}

char *char_to_base8(char c) {
    int ascii = (int) c;
    static char result[4];
    int i = 2;

    while (i >= 0) {
        result[i--] = (ascii % 8) + '0';
        ascii /= 8;
    }
    result[3] = '\0';

    return result;
}

ErrorCode process_a_option(FILE *input, FILE *output) {
    char *lexeme;
    char *output_buffer = malloc(10000);  // Большой буфер для вывода
    if (!output_buffer) {
        return ERR_MEMORY_ERROR;
    }
    size_t output_len = 0;
    int count = 0;

    while ((lexeme = read_lexeme(input)) != NULL) {
        count++;

        for (int i = 0; lexeme[i] != '\0'; i++) {
            char c = lexeme[i];

            if (count % 10 == 0) {
                if (isalpha(c)) {
                    output_len += sprintf(output_buffer + output_len, "%s", char_to_base4(c));
                } else {
                    output_len += sprintf(output_buffer + output_len, "%c", c);
                }
            } else if (count % 2 == 0 && count % 10 != 0) {
                if (isalpha(c)) {
                    output_len += sprintf(output_buffer + output_len, "%c", isupper(c) ? tolower(c) : toupper(c));
                } else {
                    output_len += sprintf(output_buffer + output_len, "%c", c);
                }
            } else if (count % 5 == 0 && count % 10 != 0) {
                if (isalpha(c)) {
                    output_len += sprintf(output_buffer + output_len, "%s", char_to_base8(c));
                } else {
                    output_len += sprintf(output_buffer + output_len, "%c", c);
                }
            } else {
                output_len += sprintf(output_buffer + output_len, "%c", c);
            }
            if (output_len >= 10000 - 100) {
                flush_buffer(output, output_buffer, &output_len);
            }

        }

        output_len += sprintf(output_buffer + output_len, " ");
        free(lexeme);
    }

    if (output_len > 0) {
        flush_buffer(output, output_buffer, &output_len);
    }
    free(output_buffer);
    return SUCCESS;
}

int main(int argc, char **argv) {
    kOpts option;

    int opt_result = GetOpts(argc, argv, &option);
    if (opt_result != SUCCESS) {
        printf("Error: Invalid arguments or option.\n");
        return opt_result;
    }

    if (option == OPT_R) {
        if (check_file_names(argv[2], argv[3]) != SUCCESS) {
            printf("Error: File names are the same.\n");
            return ERR_SAME_FILE_NAMES;
        }

        FILE *file1 = fopen(argv[2], "r");
        FILE *file2 = fopen(argv[3], "r");
        FILE *output = fopen(argv[4], "w");

        if (!file1 || !file2 || !output) {
            printf("Error: Could not open one or more files.\n");
            return ERR_FILE_FAIL;
        }

        ErrorCode err = process_r_option(file1, file2, output);
        if (err == ERR_MEMORY_ERROR) {
            printf("Memory error");
            return err;
        }

        fclose(file1);
        fclose(file2);
        fclose(output);
    } else if (option == OPT_A) {
        FILE *input = fopen(argv[2], "r");
        FILE *output = fopen(argv[3], "w");

        if (!input || !output) {
            printf("Error: Could not open one or more files.\n");
            return ERR_FILE_FAIL;
        }


        ErrorCode err = process_a_option(input, output);
        if (err == ERR_MEMORY_ERROR) {
            printf("Memory error");
            return err;
        }

        fclose(input);
        fclose(output);
    }

    printf("Processing completed successfully.\n");
    return SUCCESS;
}
