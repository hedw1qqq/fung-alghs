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

        if (read_count != 9) {
            free(student.grades);
            free(student.first_name);
            free(student.last_name);
            free(student.group);
            continue;
        }

        if (!is_valid_name(student.first_name) ||
            !is_valid_name(student.last_name) ||
            !is_valid_group(student.group)) {
            free(student.grades);
            free(student.first_name);
            free(student.last_name);
            free(student.group);
            continue;
        }

        int valid_grades = 1;
        for (int j = 0; j < 5; j++) {
            if (student.grades[j] < 2 || student.grades[j] > 5) {
                valid_grades = 0;
                break;
            }
        }

        if (!valid_grades) {
            free(student.grades);
            free(student.first_name);
            free(student.last_name);
            free(student.group);
            continue;
        }


        Student *temp = realloc(student_array->students,
                                (student_array->size + 1) * sizeof(Student));
        if (!temp) {
            free(student.grades);
            free(student.first_name);
            free(student.last_name);
            free(student.group);
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
void free_students(StudentArray *student_array) {
    for (int i = 0; i < student_array->size; i++) {
        free(student_array->students[i].first_name);
        free(student_array->students[i].last_name);
        free(student_array->students[i].group);
        free(student_array->students[i].grades);
    }
    free(student_array->students);
}

double calculate_average(const Student *student) {
    double total = 0;
    for (int i = 0; i < 5; i++) {
        total += student->grades[i];
    }
    return total / 5.0;
}

Student *find_student_by_id(const StudentArray *student_array, unsigned int id) {
    for (int i = 0; i < student_array->size; i++) {
        if (student_array->students[i].id == id) {
            return &student_array->students[i];
        }
    }
    return NULL;
}


errors save_to_log(const char *log_filename, const Student *student) {
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

void print_above_average_students(const StudentArray *student_array) {
    double total_average = 0;
    for (int i = 0; i < student_array->size; i++) {
        total_average += calculate_average(&student_array->students[i]);
    }
    total_average /= student_array->size;

    printf("Students with above average grade (%.2f):\n", total_average);
    for (int i = 0; i < student_array->size; i++) {
        if (calculate_average(&student_array->students[i]) > total_average) {
            printf("%s %s, Group: %s, Average Grade: %.2f\n",
                   student_array->students[i].first_name,
                   student_array->students[i].last_name,
                   student_array->students[i].group,
                   calculate_average(&student_array->students[i]));
        }
    }
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
    errors result = load_students(filename_input, &student_array);
    if (result != ok) {
        printf("Error: %d", result);
        return result;
    }

    while (1) {
        printf("Enter command (1: Search by ID, 2: Print students above average, 3: Exit):\n ");
        int command;
        scanf("%d", &command);

        if (command == 1) {
            printf("Enter student ID:\n");
            unsigned int id;
            scanf("%u", &id);
            Student *student = find_student_by_id(&student_array, id);
            if (student) {
                printf("Found student: %s %s, Group: %s, Average Grade: %.2f \n", student->first_name,
                       student->last_name, student->group,
                       calculate_average(student));
                save_to_log(filename_output, student);
            } else {
                printf("Student with ID %u not found.\n", id);
            }
        } else if (command == 2) {
            print_above_average_students(&student_array);
        } else if (command == 3) {
            break;
        } else {
            printf("Invalid command. Please try again.\n");
        }
    }

    free_students(&student_array);
    return 0;
}