#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    unsigned int id;
    char *name;
    char *surname;
    double salary;
} Employee;

typedef enum {
    ok,
    err_open,
    err_memory,
    err_args,
    err_files,
    err_format
} error_code;

int compare_employees_asc(const void *a, const void *b) {
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

int compare_employees_desc(const void *a, const void *b) {
    return compare_employees_asc(b, a);
}

int validate_name(const char *name) {
    if (!name || strlen(name) == 0) return 0;

    for (int i = 0; name[i]; i++) {
        if (!isalpha(name[i])) {
            return 0;
        }
    }
    return 1;
}

error_code read_employees(const char *input_file, Employee **employees, int *num_employees) {
    FILE *fp = fopen(input_file, "r");
    if (!fp) {
        return err_open;
    }

    int capacity = 16;
    *employees = malloc(capacity * sizeof(Employee));
    if (!*employees) {
        fclose(fp);
        return err_memory;
    }

    *num_employees = 0;
    char line[1024];

    while (fgets(line, sizeof(line), fp)) {
        if (*num_employees == capacity) {
            capacity *= 2;
            Employee *temp = realloc(*employees, capacity * sizeof(Employee));
            if (!temp) {
                for (int i = 0; i < *num_employees; i++) {
                    free((*employees)[i].name);
                    free((*employees)[i].surname);
                }
                free(*employees);
                fclose(fp);
                return err_memory;
            }
            *employees = temp;
        }

        Employee emp;
        emp.name = malloc(256 * sizeof(char));
        emp.surname = malloc(256 * sizeof(char));

        if (!emp.name || !emp.surname) {
            free(emp.name);
            free(emp.surname);
            for (int i = 0; i < *num_employees; i++) {
                free((*employees)[i].name);
                free((*employees)[i].surname);
            }
            free(*employees);
            fclose(fp);
            return err_memory;
        }

        if (sscanf(line, "%u %255s %255s %lf", &emp.id, emp.name, emp.surname, &emp.salary) != 4) {
            free(emp.name);
            free(emp.surname);
            continue;
        }

        if (emp.salary < 0 || !validate_name(emp.name) || !validate_name(emp.surname)) {
            free(emp.name);
            free(emp.surname);
            continue;
        }

        (*employees)[*num_employees] = emp;
        (*num_employees)++;
    }

    fclose(fp);
    return ok;
}

error_code write_employees(FILE *fp, const Employee *employees, int num_employees) {
    for (int i = 0; i < num_employees; i++) {
        if (fprintf(fp, "%u %s %s %.2f\n",
                    employees[i].id,
                    employees[i].name,
                    employees[i].surname,
                    employees[i].salary)< 0)  {
            return err_format;
        }
    }
    return ok;
}

error_code sort_and_write_employees(const char *input_file, const char *output_file, char sort_flag) {
    Employee *employees = NULL;
    int num_employees = 0;

    error_code status = read_employees(input_file, &employees, &num_employees);
    if (status != ok) {
        return status;
    }

    if (sort_flag == 'd' || sort_flag == 'D') {
        qsort(employees, num_employees, sizeof(Employee), compare_employees_desc);
    } else {
        qsort(employees, num_employees, sizeof(Employee), compare_employees_asc);
    }

    FILE *fp = fopen(output_file, "w");
    if (!fp) {
        for (int i = 0; i < num_employees; i++) {
            free(employees[i].name);
            free(employees[i].surname);
        }
        free(employees);
        return err_open;
    }

    status = write_employees(fp, employees, num_employees);

    fclose(fp);
    for (int i = 0; i < num_employees; i++) {
        free(employees[i].name);
        free(employees[i].surname);
    }
    free(employees);

    return status;
}

int check_file_names(const char *file1, const char *file2) {
    return strcmp(file1, file2);
}


const char *find_file_name(const char *file_string) {
    const char *file_name = strrchr(file_string, '\\');
    return file_name ? file_name + 1 : file_string;
}


error_code validate_params(int argc, char **argv, char **input_file, char *sort_flag, char **output_file) {
    if (argc != 4) {
        return err_args;
    }

    *input_file = (char *)find_file_name(argv[1]);
    *output_file = (char *)find_file_name(argv[3]);

    if (!check_file_names(*input_file, *output_file)) {
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

    error_code status = validate_params(argc, argv, &input_file, &sort_flag, &output_file);
    if (status != ok) {
        switch (status) {
            case err_args:
                fprintf(stderr, "usage: %s <input_file> <-a|-d> <output_file>\n", argv[0]);
                break;
            case err_files:
                fprintf(stderr, "error: input and output files must be different\n");
                break;
            default:
                fprintf(stderr, "unknown error occurred\n");
        }
        return status;
    }

    status = sort_and_write_employees(input_file, output_file, sort_flag);
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
            case err_format:
                fprintf(stderr, "error: writing to file failed\n");
                break;
            default:
                fprintf(stderr, "unknown error occurred\n");
        }
    }

    return status;
}