#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>


typedef struct {
    char* name;
    int value;
} MemoryCell;


typedef struct {
    MemoryCell* cells;
    int capacity;
    int size;
} Interpreter;


typedef enum {
    SUCCESS = 0,
    ERROR_MEMORY_ALLOCATION,
    ERROR_FILE_OPEN,
    ERROR_FILE_READ,
    ERROR_INVALID_INSTRUCTION,
    ERROR_VARIABLE_NOT_FOUND,
    ERROR_DIVISION_BY_ZERO,
    ERROR_NUMBER_OVERFLOW,
    ERROR_INVALID_NUMBER
} StatusCode;

StatusCode safe_strtol(const char* str, int* result) {
    char* endptr;
    long val = strtol(str, &endptr, 10);

    if (*endptr != '\0' && !isspace(*endptr)) {
        return ERROR_INVALID_NUMBER;
    }

    if (val > INT_MAX || val < INT_MIN) {
        return ERROR_NUMBER_OVERFLOW;
    }

    if (endptr == str) {
        return ERROR_INVALID_NUMBER;
    }

    *result = (int)val;
    return SUCCESS;
}

int compare_cells(const void* a, const void* b) {
    const MemoryCell* cell_a = (const MemoryCell*)a;
    const MemoryCell* cell_b = (const MemoryCell*)b;
    return strcmp(cell_a->name, cell_b->name);
}

StatusCode interpreter_init(Interpreter* interpreter) {
    interpreter->capacity = 8;
    interpreter->size = 0;
    interpreter->cells = malloc(interpreter->capacity * sizeof(MemoryCell));

    if (!interpreter->cells) {
        return ERROR_MEMORY_ALLOCATION;
    }

    return SUCCESS;
}

StatusCode find_variable(const Interpreter* interpreter, const char* name, int* index) {
    int left = 0;
    int right = interpreter->size - 1;

    while (left <= right) {
        int mid = (left + right) / 2;
        int cmp = strcmp(interpreter->cells[mid].name, name);

        if (cmp == 0) {
            *index = mid;
            return SUCCESS;
        }

        if (cmp < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    *index = left;
    return ERROR_VARIABLE_NOT_FOUND;
}

StatusCode add_variable(Interpreter* interpreter, const char* name, int value) {
    int index;
    if (find_variable(interpreter, name, &index) == SUCCESS) {
        interpreter->cells[index].value = value;
        return SUCCESS;
    }

    if (interpreter->size == interpreter->capacity) {
        int new_capacity = interpreter->capacity * 2;
        MemoryCell* new_cells = realloc(interpreter->cells, new_capacity * sizeof(MemoryCell));

        if (!new_cells) {
            return ERROR_MEMORY_ALLOCATION;
        }

        interpreter->cells = new_cells;
        interpreter->capacity = new_capacity;
    }

    interpreter->cells[interpreter->size].name = strdup(name);
    if (!interpreter->cells[interpreter->size].name) {
        return ERROR_MEMORY_ALLOCATION;
    }
    interpreter->cells[interpreter->size].value = value;
    interpreter->size++;

    qsort(interpreter->cells, interpreter->size, sizeof(MemoryCell), compare_cells);

    return SUCCESS;
}

StatusCode get_variable_value(const Interpreter* interpreter, const char* name, int* value) {
    int index;
    StatusCode status = find_variable(interpreter, name, &index);

    if (status == SUCCESS) {
        *value = interpreter->cells[index].value;
    }

    return status;
}

StatusCode check_arithmetic_overflow(int a, int b, char op, int* result) {
    switch (op) {
        case '+':
            if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b)) {
                return ERROR_NUMBER_OVERFLOW;
            }
            *result = a + b;
            break;

        case '-':
            if ((b < 0 && a > INT_MAX + b) || (b > 0 && a < INT_MIN + b)) {
                return ERROR_NUMBER_OVERFLOW;
            }
            *result = a - b;
            break;

        case '*':
            if (b != 0) {
                if (a > INT_MAX / b || a < INT_MIN / b) {
                    return ERROR_NUMBER_OVERFLOW;
                }
            }
            *result = a * b;
            break;

        case '/':
            if (b == 0) {
                return ERROR_DIVISION_BY_ZERO;
            }
            if (a == INT_MIN && b == -1) {
                return ERROR_NUMBER_OVERFLOW;
            }
            *result = a / b;
            break;

        case '%':
            if (b == 0) {
                return ERROR_DIVISION_BY_ZERO;
            }
            if (a == INT_MIN && b == -1) {
                return ERROR_NUMBER_OVERFLOW;
            }
            *result = a % b;
            break;

        default:
            return ERROR_INVALID_INSTRUCTION;
    }

    return SUCCESS;
}

void print_all_variables(const Interpreter* interpreter) {
    for (int i = 0; i < interpreter->size; i++) {
        printf("%s = %d\n", interpreter->cells[i].name, interpreter->cells[i].value);
    }
}

StatusCode process_instruction(Interpreter* interpreter, const char* instruction) {
    char* line = strdup(instruction);
    if (!line) {
        return ERROR_MEMORY_ALLOCATION;
    }

    char* semicolon = strchr(line, ';');
    if (semicolon) {
        *semicolon = '\0';
    }

    if (strncmp(line, "print", 5) == 0) {
        if (strlen(line) == 5) {
            print_all_variables(interpreter);
        } else {
            char* var_name = line + 6;
            int value;
            StatusCode status = get_variable_value(interpreter, var_name, &value);
            if (status == SUCCESS) {
                printf("%s = %d\n", var_name, value);
            } else {
                printf("Error: Variable '%s' not found\n", var_name);
            }
        }
        free(line);
        return SUCCESS;
    }

    char* equals = strchr(line, '=');
    if (!equals) {
        free(line);
        return ERROR_INVALID_INSTRUCTION;
    }

    *equals = '\0';
    char* var_name = line;
    char* expression = equals + 1;

    while (*var_name && isspace(*var_name)) var_name++;
    char* end = var_name + strlen(var_name) - 1;
    while (end > var_name && isspace(*end)) *end-- = '\0';

    while (*expression && isspace(*expression)) expression++;
    end = expression + strlen(expression) - 1;
    while (end > expression && isspace(*end)) *end-- = '\0';

    int value;
    StatusCode status;


    if (strlen(expression) == 0) {
        free(line);
        return ERROR_INVALID_INSTRUCTION;
    }


    status = safe_strtol(expression, &value);
    if (status == SUCCESS) {
        status = add_variable(interpreter, var_name, value);
        free(line);
        return status;
    } else if (status != ERROR_INVALID_NUMBER) {
        free(line);
        return status;
    }

    char* operator = strpbrk(expression, "+-*/%");
    if (operator) {
        char op = *operator;
        *operator = '\0';
        char* left_str = expression;
        char* right_str = operator + 1;

        int left_val;
        if (isdigit(*left_str) || *left_str == '-') {
            status = safe_strtol(left_str, &left_val);
            if (status != SUCCESS) {
                free(line);
                return status;
            }
        } else {
            status = get_variable_value(interpreter, left_str, &left_val);
            if (status != SUCCESS) {
                free(line);
                return status;
            }
        }

        int right_val;
        if (isdigit(*right_str) || *right_str == '-') {
            status = safe_strtol(right_str, &right_val);
            if (status != SUCCESS) {
                free(line);
                return status;
            }
        } else {
            status = get_variable_value(interpreter, right_str, &right_val);
            if (status != SUCCESS) {
                free(line);
                return status;
            }
        }

        int result;
        status = check_arithmetic_overflow(left_val, right_val, op, &result);
        if (status != SUCCESS) {
            free(line);
            return status;
        }

        status = add_variable(interpreter, var_name, result);
        free(line);
        return status;
    } else {

        status = get_variable_value(interpreter, expression, &value);
        if (status != SUCCESS) {
            free(line);
            return status;
        }

        status = add_variable(interpreter, var_name, value);
        free(line);
        return status;
    }
}

void interpreter_cleanup(Interpreter* interpreter) {
    for (int i = 0; i < interpreter->size; i++) {
        free(interpreter->cells[i].name);
    }
    free(interpreter->cells);
    interpreter->cells = NULL;
    interpreter->size = 0;
    interpreter->capacity = 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return ERROR_FILE_OPEN;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        printf("Error: Cannot open file '%s'\n", argv[1]);
        return ERROR_FILE_READ;
    }

    Interpreter interpreter;
    StatusCode status = interpreter_init(&interpreter);
    if (status != SUCCESS) {
        printf("Error: Failed to initialize interpreter\n");
        fclose(file);
        return status;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;


        if (strlen(line) == 0) continue;

        status = process_instruction(&interpreter, line);
        switch(status) {
            case SUCCESS:
                break;
            case ERROR_NUMBER_OVERFLOW:
                printf("Error: Number overflow in instruction: %s\n", line);
                break;
            case ERROR_DIVISION_BY_ZERO:
                printf("Error: Division by zero in instruction: %s\n", line);
                break;
            case ERROR_INVALID_NUMBER:
                printf("Error: Invalid number format in instruction: %s\n", line);
                break;
            case ERROR_VARIABLE_NOT_FOUND:
                printf("Error: Variable not found in instruction: %s\n", line);
                break;
            default:
                printf("Error processing instruction: %s\n", line);
        }
        if (status != SUCCESS) break;
    }

    interpreter_cleanup(&interpreter);
    fclose(file);
    return 0;
}
