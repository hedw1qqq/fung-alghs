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
    ok,
    err_open,    // ошибка открытия файла
    err_memory,  // ошибка выделения памяти
    err_args,    // неверные аргументы
    err_files    // проблемы с файлами
} error_code;

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

error_code read_employees(const char *input_file, Employee **employees, size_t *num_employees) {
    FILE *fp = fopen(input_file, "r");
    if (!fp) {
        return err_open;
    }

    size_t capacity = 16;
    *employees = (Employee *) malloc(capacity * sizeof(Employee));
    if (!*employees) {
        fclose(fp);
        return err_memory;
    }

    *num_employees = 0;
    while (fscanf(fp, "%u %63s %63s %lf", &(*employees)[*num_employees].id,
                  (*employees)[*num_employees].name,
                  (*employees)[*num_employees].surname,
                  &(*employees)[*num_employees].salary) == 4) {
        (*num_employees)++;
        if (*num_employees == capacity) {
            capacity *= 2;
            Employee *temp = (Employee *) realloc(*employees, capacity * sizeof(Employee));
            if (!temp) {
                free(*employees);
                fclose(fp);
                return err_memory;
            }
            *employees = temp;
        }
    }

    fclose(fp);
    return ok;
}

error_code sort_and_write_employees(const char *input_file, const char *output_file, int sort_flag) {
    Employee *employees = NULL;
    size_t num_employees = 0;

    error_code status = read_employees(input_file, &employees, &num_employees);
    if (status != ok) {
        return status;
    }

    qsort(employees, num_employees, sizeof(Employee), compare_employees);

    FILE *fp = fopen(output_file, "w");
    if (!fp) {
        free(employees);
        return err_open;
    }

    if (sort_flag == 'd' || sort_flag == 'D') {
        for (size_t i = num_employees; i > 0; i--) {
            fprintf(fp, "%u %s %s %.2f\n",
                    employees[i - 1].id,
                    employees[i - 1].name,
                    employees[i - 1].surname,
                    employees[i - 1].salary);
        }
    } else {
        for (size_t i = 0; i < num_employees; i++) {
            fprintf(fp, "%u %s %s %.2f\n",
                    employees[i].id,
                    employees[i].name,
                    employees[i].surname,
                    employees[i].salary);
        }
    }

    fclose(fp);
    free(employees);
    return ok;
}

int check_file_names(const char *file1, const char *file2) {
    if (strcmp(file1, file2) == 0) {
        return err_files;
    }
    return ok;
}

const char *find_file_name(const char *file_string) {
    const char *file_name = strrchr(file_string, '\\');
    if (file_name != NULL)
        return file_name + 1;
    return file_string;
}

error_code validate_params(int argc, char **argv, char **input_file, char *sort_flag, char **output_file) {
    if (argc != 4) {
        return err_args;
    }

    *input_file = (char *) find_file_name(argv[1]);
    *output_file = (char *) find_file_name(argv[3]);

    if (check_file_names(*input_file, *output_file) == err_files) {
        return err_files;
    }

    if (strlen(argv[2]) != 2 || (argv[2][0] != '-' && argv[2][0] != '/')) {
        return err_args;
    }

    *sort_flag = argv[2][1];
    if (*sort_flag != 'a' && *sort_flag != 'A' &&
        *sort_flag != 'd' && *sort_flag != 'D') {
        return err_args;
    }

    return ok;
}

int main(int argc, char *argv[]) {
    char *input_file;
    char *output_file;
    char sort_flag;

    error_code param_status = validate_params(argc, argv, &input_file, &sort_flag, &output_file);
    if (param_status != ok) {
        switch (param_status) {
            case err_args:
                fprintf(stderr, "usage: %s <input_file> <-a|-d> <output_file>\n", argv[0]);
                break;
            case err_files:
                fprintf(stderr, "error: input and output files must be different\n");
                break;
            default:
                fprintf(stderr, "unknown error occurred\n");
        }
        return param_status;
    }

    error_code status = sort_and_write_employees(input_file, output_file, sort_flag);
    if (status == ok) {
        printf("sorting completed successfully\n");
    } else {
        switch (status) {
            case err_open:
                fprintf(stderr, "error: could not access file\n");
                break;
            case err_memory:
                fprintf(stderr, "error: memory allocation failed\n");
                break;
            default:
                fprintf(stderr, "unknown error occurred\n");
        }
    }

    return status;
}