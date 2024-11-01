#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024


enum {
    ok,
    invalid_args,
    same_filenames,
    file_open_error
};

int check_file_names(const char *file1, const char *file2) {
    return strcmp(file1, file2);
}

const char *find_file_name(const char *file_string) {
    const char *file_name = strrchr(file_string, '\\');
    if (file_name != NULL)
        return file_name + 1;
    return file_string;
}

int parse_expression(const char *str, FILE *output) {
    int depth = 0;

    for (int index = 0; str[index] != '\0'; index++) {
        char c = str[index];

        if (isalpha(c)) {
            for (int i = 0; i < depth; i++) {
                fprintf(output, "    ");
            }
            fprintf(output, "%c\n", c);
        } else if (c == '(') {
            depth++;
        } else if (c == ')') {
            depth--;
        }
    }

    return ok;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return invalid_args;
    }
    const char *filename_input = find_file_name(argv[1]);
    const char *filename_out = find_file_name(argv[2]);
    if (!check_file_names(filename_out, filename_input)) {
        fprintf(stderr, "Input and output files have the same names\n");
        return same_filenames;
    }

    FILE *input = fopen(filename_input, "r");
    if (!input) {
        fprintf(stderr, "Error opening input file\n");
        return file_open_error;
    }

    FILE *output = fopen(filename_out, "w");
    if (!output) {
        fprintf(stderr, "Error opening output file\n");
        fclose(input);
        return file_open_error;
    }

    char buffer[MAX_LINE_LENGTH];
    int expression_number = 1;
    while (fgets(buffer, sizeof(buffer), input)) {
        fprintf(output, "Expression #%d\n", expression_number);

        parse_expression(buffer, output);
        fprintf(output, "\n");

        expression_number++;
    }

    fclose(input);
    fclose(output);

    return 0;
}