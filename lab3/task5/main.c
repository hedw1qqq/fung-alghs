#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    ok,
    file_error,
    memory_error,
    invalid_input,
    same_files_name
} errors;

typedef struct {
    unsigned int id;
    char *first_name;
    char *last_name;
    char *group;
    unsigned char *grades;
} Student;

typedef struct {
    Student *students;
    int size;
} StudentArray;


int is_valid_name(const char *name) {
    if (!name || strlen(name) == 0) return 0;
    for (int i = 0; i < strlen(name); i++) {
        if (!isalpha(name[i])) return 0;
    }
    return 1;
}

int is_valid_group(const char *group) {
    if (!group || strlen(group) == 0) return 0;
    for (int i = 0; i < strlen(group); i++) {
        if (!isalpha(group[i]) && !isdigit(group[i])) return 0;
    }
    return 1;
}

int check_file_names(const char *file1, const char *file2) {
    return strcmp(file1, file2);
}

const char *find_file_name(const char *file_string) {
    const char *file_name = strrchr(file_string, '\\');
    if (file_name != NULL)
        return file_name + 1;
    return file_string;
}

void free_student(Student *student) {
    if (student) {
        free(student->first_name);
        free(student->last_name);
        free(student->group);
        free(student->grades);
    }
}

void free_students(StudentArray *student_array) {
    if (student_array && student_array->students) {
        for (int i = 0; i < student_array->size; i++) {
            free_student(&student_array->students[i]);
        }
        free(student_array->students);
        student_array->students = NULL;
        student_array->size = 0;
    }
}

int compare_by_id(const void *a, const void *b) {
    const Student *student_a = (const Student *) a;
    const Student *student_b = (const Student *) b;
    if (student_a->id < student_b->id) return -1;
    if (student_a->id > student_b->id) return 1;
    return 0;
}

int compare_by_first_name(const void *a, const void *b) {
    const Student *student_a = (const Student *) a;
    const Student *student_b = (const Student *) b;
    return strcmp(student_a->first_name, student_b->first_name);
}

int compare_by_last_name(const void *a, const void *b) {
    const Student *student_a = (const Student *) a;
    const Student *student_b = (const Student *) b;
    return strcmp(student_a->last_name, student_b->last_name);
}

int compare_by_group(const void *a, const void *b) {
    const Student *student_a = (const Student *) a;
    const Student *student_b = (const Student *) b;
    return strcmp(student_a->group, student_b->group);
}

double calculate_average(const Student *student) {
    if (!student || !student->grades) return 0.0;
    double sum = 0.0;
    for (int i = 0; i < 5; i++) {
        sum += student->grades[i];
    }
    return sum / 5.0;
}

double calculate_total_average(const StudentArray *student_array) {
    if (!student_array || student_array->size == 0) return 0.0;
    double total = 0.0;
    for (int i = 0; i < student_array->size; i++) {
        total += calculate_average(&student_array->students[i]);
    }
    return total / student_array->size;
}

Student *find_student_by_id(const StudentArray *student_array, unsigned int id) {
    for (int i = 0; i < student_array->size; i++) {
        if (student_array->students[i].id == id) {
            return &student_array->students[i];
        }
    }
    return NULL;
}

void search_by_first_name(const StudentArray *student_array, const char *first_name) {
    int found = 0;
    for (int i = 0; i < student_array->size; i++) {
        if (strcmp(student_array->students[i].first_name, first_name) == 0) {
            printf("Found student: ID: %u, %s %s, Group: %s\n",
                   student_array->students[i].id,
                   student_array->students[i].first_name,
                   student_array->students[i].last_name,
                   student_array->students[i].group);
            found = 1;
        }
    }
    if (!found) {
        printf("No students found with first name: %s\n", first_name);
    }
}

void search_by_last_name(const StudentArray *student_array, const char *last_name) {
    int found = 0;
    for (int i = 0; i < student_array->size; i++) {
        if (strcmp(student_array->students[i].last_name, last_name) == 0) {
            printf("Found student: ID: %u, %s %s, Group: %s\n",
                   student_array->students[i].id,
                   student_array->students[i].first_name,
                   student_array->students[i].last_name,
                   student_array->students[i].group);
            found = 1;
        }
    }
    if (!found) {
        printf("No students found with last name: %s\n", last_name);
    }
}

void search_by_group(const StudentArray *student_array, const char *group) {
    int found = 0;
    for (int i = 0; i < student_array->size; i++) {
        if (strcmp(student_array->students[i].group, group) == 0) {
            printf("Found student: ID: %u, %s %s, Group: %s\n",
                   student_array->students[i].id,
                   student_array->students[i].first_name,
                   student_array->students[i].last_name,
                   student_array->students[i].group);
            found = 1;
        }
    }
    if (!found) {
        printf("No students found in group: %s\n", group);
    }
}

errors load_students(const char *filename, StudentArray *student_array) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return file_error;
    }

    student_array->size = 0;
    student_array->students = NULL;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        Student student;
        student.grades = malloc(5 * sizeof(unsigned char));
        if (!student.grades) {
            fclose(file);
            return memory_error;
        }

        student.first_name = malloc(50 * sizeof(char));
        student.last_name = malloc(50 * sizeof(char));
        student.group = malloc(50 * sizeof(char));

        if (!student.first_name || !student.last_name || !student.group) {
            free(student.grades);
            if (student.first_name) free(student.first_name);
            if (student.last_name) free(student.last_name);
            if (student.group) free(student.group);
            fclose(file);
            return memory_error;
        }

        int read_count = sscanf(line, "%u %49s %49s %49s %hhu %hhu %hhu %hhu %hhu",
                                &student.id,
                                student.first_name,
                                student.last_name,
                                student.group,
                                &student.grades[0],
                                &student.grades[1],
                                &student.grades[2],
                                &student.grades[3],
                                &student.grades[4]);

        if (read_count != 9 ||
            !is_valid_name(student.first_name) ||
            !is_valid_name(student.last_name) ||
            !is_valid_group(student.group)) {
            free_student(&student);
            continue;
        }

        int valid_grades = 1;
        for (int i = 0; i < 5; i++) {
            if (student.grades[i] < 2 || student.grades[i] > 5) {
                valid_grades = 0;
                break;
            }
        }

        if (!valid_grades) {
            free_student(&student);
            continue;
        }

        Student *temp = realloc(student_array->students,
                                (student_array->size + 1) * sizeof(Student));
        if (!temp) {
            free_student(&student);
            fclose(file);
            return memory_error;
        }

        student_array->students = temp;
        student_array->students[student_array->size] = student;
        student_array->size++;
    }

    fclose(file);
    return ok;
}

errors save_to_log(const char *log_filename, const Student *student) {
    if (!student) return invalid_input;

    FILE *log_file = fopen(log_filename, "a");
    if (!log_file) {
        return file_error;
    }

    fprintf(log_file, "ID: %u, Full Name: %s %s, Group: %s, Average Grade: %.2f\n",
            student->id,
            student->first_name,
            student->last_name,
            student->group,
            calculate_average(student));

    fclose(log_file);
    return ok;
}


void print_above_average_students(const StudentArray *student_array, const char *filename) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        return;
    }
    if (!student_array || student_array->size == 0) {
        printf("No students in array.\n");
        return;
    }

    double total_average = calculate_total_average(student_array);
    fprintf(file, "Students with above average grade (%.2f):\n", total_average);
    printf("\nStudents with above average grade (%.2f):\n", total_average);
    for (int i = 0; i < student_array->size; i++) {
        double student_average = calculate_average(&student_array->students[i]);
        if (student_average > total_average) {
            printf("%s %s, Group: %s, Average Grade: %.2f\n",
                   student_array->students[i].first_name,
                   student_array->students[i].last_name,
                   student_array->students[i].group,
                   student_average);
            fprintf(file, "%s %s, Group: %s, Average Grade: %.2f\n",
                    student_array->students[i].first_name,
                    student_array->students[i].last_name,
                    student_array->students[i].group,
                    student_average);
        }
    }
}

void sort_and_print_students(StudentArray *student_array, int (*compare)(const void *, const void *)) {
    if (!student_array || !student_array->students || student_array->size == 0) {
        printf("No students to sort.\n");
        return;
    }

    qsort(student_array->students, student_array->size, sizeof(Student), compare);

    printf("\nSorted student list:\n");
    for (int i = 0; i < student_array->size; i++) {
        printf("ID: %u, %s %s, Group: %s\n",
               student_array->students[i].id,
               student_array->students[i].first_name,
               student_array->students[i].last_name,
               student_array->students[i].group);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <path to file> <path to log file>\n", argv[0]);
        return invalid_input;
    }

    const char *filename_input = find_file_name(argv[1]);
    const char *filename_output = find_file_name(argv[2]);

    if (!check_file_names(filename_input, filename_output)) {
        fprintf(stderr, "Input and output files have the same names\n");
        return same_files_name;
    }

    StudentArray student_array = {NULL, 0};
    errors result = load_students(argv[1], &student_array);
    if (result != ok) {
        printf("Error loading students: %d\n", result);
        return result;
    }

    while (1) {
        printf("\nMenu:\n");
        printf("1: Search by ID\n");
        printf("2: Search by Last Name\n");
        printf("3: Search by First Name\n");
        printf("4: Search by Group\n");
        printf("5: Sort by ID\n");
        printf("6: Sort by Last Name\n");
        printf("7: Sort by First Name\n");
        printf("8: Sort by Group\n");
        printf("9: Print students above average\n");
        printf("0: Exit\n");
        printf("Enter command: ");

        int command;
        if (scanf("%d", &command) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (command) {
            case 0:
                free_students(&student_array);
                return ok;

            case 1: {
                printf("Enter student ID: ");
                unsigned int id;
                if (scanf("%u", &id) == 1) {
                    Student *student = find_student_by_id(&student_array, id);
                    if (student) {
                        printf("Found student: %s %s, Group: %s, Average Grade: %.2f\n",
                               student->first_name, student->last_name,
                               student->group, calculate_average(student));
                        save_to_log(argv[2], student);
                    } else {
                        printf("Student with ID %u not found.\n", id);
                    }
                } else {
                    printf("Invalid ID format.\n");
                    while (getchar() != '\n');
                }
                break;
            }

            case 2: {
                printf("Enter last name: ");
                char last_name[50];
                scanf("%49s", last_name);
                search_by_last_name(&student_array, last_name);
                break;
            }

            case 3: {
                printf("Enter first name: ");
                char first_name[50];
                scanf("%49s", first_name);
                search_by_first_name(&student_array, first_name);
                break;
            }

            case 4: {
                printf("Enter group: ");
                char group[50];
                scanf("%49s", group);
                search_by_group(&student_array, group);
                break;
            }

            case 5:
                sort_and_print_students(&student_array, compare_by_id);
                break;

            case 6:
                sort_and_print_students(&student_array, compare_by_last_name);
                break;

            case 7:
                sort_and_print_students(&student_array, compare_by_first_name);
                break;

            case 8:
                sort_and_print_students(&student_array, compare_by_group);
                break;

            case 9:
                print_above_average_students(&student_array, filename_output);
                break;

            default:
                printf("Invalid command. Please enter a number between 0 and 9.\n");
                break;
        }
    }

    free_students(&student_array);
    return ok;
}