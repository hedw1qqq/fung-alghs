#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

typedef struct {
    unsigned int id;
    char name[1024];
    char surname[1024];
    double salary;
} Employee;

typedef enum {
    OK,
    ERROR_FILE_OPEN,
    ERROR_MEMORY_ALLOCATION,
    ERROR_FILE_WRITE,
    ERROR_INVALID_ARGS,
    ERROR_INVALID_INPUT_FILE,
    ERROR_INVALID_OUTPUT_FILE,
    ERROR_INPUT_OUTPUT_SAME,
    ERROR_INVALID_SORT_FLAG
} ErrorCode;

int compare_employees(const void *a, const void *b) {
    const Employee *emp1 = (const Employee *) a;
    const Employee *emp2 = (const Employee *) b;

    if (emp1->salary != emp2->salary) {
        return (emp1->salary < emp2->salary) ? -1 : 1;
    }

    int surname_cmp = strcmp(emp1->surname, emp2->surname);
    if (surname_cmp != 0) {
        return surname_cmp;
    }

    int name_cmp = strcmp(emp1->name, emp2->name);
    if (name_cmp != 0) {
        return name_cmp;
    }

    return (emp1->id < emp2->id) ? -1 : 1;
}

ErrorCode read_employees(const char *input_file, Employee **employees, size_t *num_employees) {
    FILE *fp = fopen(input_file, "r");
    if (!fp) {
        return ERROR_FILE_OPEN;
    }

    size_t capacity = 16;
    *employees = (Employee *) malloc(capacity * sizeof(Employee));
    if (!*employees) {
        fclose(fp);
        return ERROR_MEMORY_ALLOCATION;
    }

    *num_employees = 0;
    while (fscanf(fp, "%u %63s %63s %lf", &(*employees)[*num_employees].id, (*employees)[*num_employees].name,
                  (*employees)[*num_employees].surname, &(*employees)[*num_employees].salary) == 4) {
        (*num_employees)++;
        if (*num_employees == capacity) {
            capacity *= 2;
            *employees = (Employee *) realloc(*employees, capacity * sizeof(Employee));
            if (!*employees) {
                fclose(fp);
                return ERROR_MEMORY_ALLOCATION;
            }
        }
    }

    fclose(fp);
    return OK;
}

ErrorCode sort_and_write_employees(const char *input_file, const char *output_file, int sort_flag) {
    Employee *employees = NULL;
    size_t num_employees = 0;

    ErrorCode status = read_employees(input_file, &employees, &num_employees);
    if (status != 0) {
        return status;
    }

    qsort(employees, num_employees, sizeof(Employee), compare_employees);

    FILE *fp = fopen(output_file, "w");
    if (!fp) {
        free(employees);
        return ERROR_FILE_WRITE;
    }

    if (sort_flag == 'd' || sort_flag == 'D') {
        for (size_t i = num_employees; i > 0; i--) {
            fprintf(fp, "%u %s %s %.2f\n", employees[i - 1].id, employees[i - 1].name, employees[i - 1].surname,
                    employees[i - 1].salary);
        }
    } else {
        for (size_t i = 0; i < num_employees; i++) {
            fprintf(fp, "%u %s %s %.2f\n", employees[i].id, employees[i].name, employees[i].surname,
                    employees[i].salary);
        }
    }

    fclose(fp);
    free(employees);
    return OK;
}

int check_file_names(const char *file1, const char *file2) {
    if (strcmp(file1, file2) == 0) {
        return ERROR_INPUT_OUTPUT_SAME;
    }
    return OK;
}

const char *find_file_name(const char *file_string) {
    const char *file_name = strrchr(file_string, '\\');
    if (file_name != NULL)
        file_name++;
    else
        file_name = file_string;
    return file_name;
}

ErrorCode validate_params(int argc, char **argv, char **input_file, char *sort_flag, char **output_file) {
    if (argc != 4) {
        return ERROR_INVALID_ARGS;
    }

    *input_file = find_file_name(argv[1]);
    *output_file = find_file_name(argv[3]);

    if (check_file_names(*input_file, *output_file) == ERROR_INPUT_OUTPUT_SAME) {
        return ERROR_INPUT_OUTPUT_SAME;
    }

    if (strlen(argv[2]) != 2 || (argv[2][0] != '-' && argv[2][0] != '/')) {
        return ERROR_INVALID_SORT_FLAG;
    }

    *sort_flag = argv[2][1];
    if (*sort_flag != 'a' && *sort_flag != 'd') {
        return ERROR_INVALID_SORT_FLAG;
    }


    return OK;

}


int main(int argc, char *argv[]) {
    char *input_file;
    char *output_file;
    char sort_flag;

    ErrorCode param_status = validate_params(argc, argv, &input_file, &sort_flag, &output_file);
    if (param_status != OK) {
        switch (param_status) {
            case ERROR_INVALID_ARGS:
                fprintf(stderr, "Usage: %s <input_file> <-a|-d> <output_file>\n", argv[0]);
                break;
            case ERROR_INPUT_OUTPUT_SAME:
                fprintf(stderr, "Error: Input and output files must be different.\n");
                break;
            case ERROR_INVALID_SORT_FLAG:
                fprintf(stderr, "Error: Invalid sort flag. Use '-a' or '-d'.\n");
                break;
            case ERROR_INVALID_INPUT_FILE:
                fprintf(stderr, "Error: Invalid input file.\n");
                break;
            case ERROR_INVALID_OUTPUT_FILE:
                fprintf(stderr, "Error: Invalid output file.\n");
                break;
            default:
                fprintf(stderr, "Unknown error occurred.\n");
        }
        return param_status;
    }

    ErrorCode status = sort_and_write_employees(input_file, output_file, sort_flag);
    if (status == OK) {
        printf("Sorting and writing completed successfully.\n");
    } else {
        switch (status) {
            case ERROR_FILE_OPEN:
                fprintf(stderr, "Error: Could not open file %s\n", input_file);
                break;
            case ERROR_MEMORY_ALLOCATION:
                fprintf(stderr, "Error: Memory allocation failed.\n");
                break;
            case ERROR_FILE_WRITE:
                fprintf(stderr, "Error: Could not write to file %s\n", output_file);
                break;
            default:
                fprintf(stderr, "Unknown error occurred.\n");
        }
    }

    return status;
}