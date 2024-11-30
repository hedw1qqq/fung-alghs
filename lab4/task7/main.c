#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

typedef struct {
    char *name;
    int value;
} MemoryCell;

typedef struct {
    MemoryCell *cells;
    size_t capacity;
    size_t size;
} Interpreter;

typedef enum {
    SUCCESS,
    ERROR_MEMORY_ALLOCATION,
    ERROR_FILE_OPEN,
    ERROR_FILE_READ,
    ERROR_INVALID_INSTRUCTION,
    ERROR_VARIABLE_NOT_FOUND,
    ERROR_DIVISION_BY_ZERO,
    ERROR_NUMBER_OVERFLOW,
    ERROR_INVALID_NUMBER
} StatusCode;

StatusCode safe_strtol(const char *str, int *result) {
    while (isspace(*str)) str++;

    int sign = 1;
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    if (!*str) {
        return ERROR_INVALID_NUMBER;
    }

    long long val = 0;
    while (*str && !isspace(*str)) {
        if (!isdigit(*str)) {
            return ERROR_INVALID_NUMBER;
        }
        val = val * 10 + (*str - '0');

        if (sign > 0 && val > INT_MAX) {
            return ERROR_NUMBER_OVERFLOW;
        }
        if (sign < 0 && val > (long long) INT_MAX + 1) {
            return ERROR_NUMBER_OVERFLOW;
        }
        str++;
    }

    while (isspace(*str)) str++;

    if (*str) {
        return ERROR_INVALID_NUMBER;
    }

    val *= sign;

    if (val > INT_MAX || val < INT_MIN) {
        return ERROR_NUMBER_OVERFLOW;
    }

    *result = (int) val;
    return SUCCESS;
}

int compare_cells(const void *a, const void *b) {
    const MemoryCell *cell_a = (const MemoryCell *) a;
    const MemoryCell *cell_b = (const MemoryCell *) b;
    return strcmp(cell_a->name, cell_b->name);
}


StatusCode interpreter_init(Interpreter *interpreter) {
    interpreter->capacity = 10;
    interpreter->size = 0;
    interpreter->cells = malloc(interpreter->capacity * sizeof(MemoryCell));

    if (!interpreter->cells) {
        return ERROR_MEMORY_ALLOCATION;
    }

    return SUCCESS;
}


StatusCode find_variable(const Interpreter *interpreter, const char *name, int *index) {
    int left = 0;
    int right = (int) interpreter->size - 1;

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

StatusCode add_variable(Interpreter *interpreter, const char *name, int value) {
    int index;
    if (find_variable(interpreter, name, &index) == SUCCESS) {
        interpreter->cells[index].value = value;
        return SUCCESS;
    }

    if (interpreter->size == interpreter->capacity) {
        size_t new_capacity = interpreter->capacity * 2;
        MemoryCell *new_cells = realloc(interpreter->cells, new_capacity * sizeof(MemoryCell));

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

StatusCode get_variable_value(const Interpreter *interpreter, const char *name, int *value) {
    int index;
    StatusCode status = find_variable(interpreter, name, &index);

    if (status == SUCCESS) {
        *value = interpreter->cells[index].value;
    }

    return status;
}

StatusCode check_arithmetic_overflow(int a, int b, char op, int *result) {
    switch (op) {
        case '+': {

            if (b > 0) {
                if (a > INT_MAX - b) return ERROR_NUMBER_OVERFLOW;
            } else {
                if (a < INT_MIN - b) return ERROR_NUMBER_OVERFLOW;
            }
            *result = a + b;
            break;
        }

        case '-': {
            if (b > 0) {
                if (a < INT_MIN + b) return ERROR_NUMBER_OVERFLOW;
            } else {
                if (a > INT_MAX + b) return ERROR_NUMBER_OVERFLOW;
            }
            *result = a - b;
            break;
        }

        case '*': {
            if (a > 0) {
                if (b > 0) {
                    if (a > INT_MAX / b) return ERROR_NUMBER_OVERFLOW;
                } else {
                    if (b < INT_MIN / a) return ERROR_NUMBER_OVERFLOW;
                }
            } else {
                if (b > 0) {
                    if (a < INT_MIN / b) return ERROR_NUMBER_OVERFLOW;
                } else {
                    if (a != 0 && b < INT_MAX / a) return ERROR_NUMBER_OVERFLOW;
                }
            }
            *result = a * b;
            break;
        }

        case '/': {
            if (b == 0) return ERROR_DIVISION_BY_ZERO;
            if (a == INT_MIN && b == -1) return ERROR_NUMBER_OVERFLOW;
            *result = a / b;
            break;
        }

        case '%': {
            if (b == 0) return ERROR_DIVISION_BY_ZERO;
            if (a == INT_MIN && b == -1) return ERROR_NUMBER_OVERFLOW;
            *result = a % b;
            break;
        }

        default:
            return ERROR_INVALID_INSTRUCTION;
    }

    return SUCCESS;
}

void print_all_variables(const Interpreter *interpreter) {
    for (size_t i = 0; i < interpreter->size; i++) {
        printf("%s = %d\n", interpreter->cells[i].name, interpreter->cells[i].value);
    }
}

StatusCode
process_arithmetic(const char *left_str, const char *right_str, char op, int *result, const Interpreter *interpreter) {
    int left_val, right_val;
    StatusCode status;

    if (isdigit(*left_str) || *left_str == '-' || *left_str == '+') {
        status = safe_strtol(left_str, &left_val);
    } else {
        status = get_variable_value(interpreter, left_str, &left_val);
        if (status == ERROR_VARIABLE_NOT_FOUND) {
            printf("Error: Undefined variable '%s'\n", left_str);
            return ERROR_VARIABLE_NOT_FOUND;
        }
    }
    if (status != SUCCESS) {
        printf("Error evaluating left operand '%s'\n", left_str);
        return status;
    }

    if (isdigit(*right_str) || *right_str == '-' || *right_str == '+') {
        status = safe_strtol(right_str, &right_val);
    } else {
        status = get_variable_value(interpreter, right_str, &right_val);
        if (status == ERROR_VARIABLE_NOT_FOUND) {
            printf("Error: Undefined variable '%s'\n", right_str);
            return ERROR_VARIABLE_NOT_FOUND;
        }
    }
    if (status != SUCCESS) {
        printf("Error evaluating right operand '%s'\n", right_str);
        return status;
    }

    status = check_arithmetic_overflow(left_val, right_val, op, result);
    if (status != SUCCESS) {
        printf("Arithmetic overflow: %d %c %d\n", left_val, op, right_val);
        return status;
    }

    return SUCCESS;
}

StatusCode process_instruction(Interpreter *interpreter, const char *instruction) {
    char *line = strdup(instruction);
    if (!line) {
        return ERROR_MEMORY_ALLOCATION;
    }

    char *semicolon = strchr(line, ';');
    if (semicolon) {
        *semicolon = '\0';
    } else {
        free(line);
        return ERROR_INVALID_INSTRUCTION;
    }
    char *start = line;
    while (isspace(*start)) start++;
    char *end = start + strlen(start) - 1;
    while (end > start && isspace(*end)) *end-- = '\0';

    if (strncmp(start, "print", 5) == 0) {
        if (strlen(start) == 5) {
            print_all_variables(interpreter);
            free(line);
            return SUCCESS;
        }
        char *var_name = start + 5;
        while (isspace(*var_name)) var_name++;

        int value;
        StatusCode status = get_variable_value(interpreter, var_name, &value);
        if (status == SUCCESS) {
            printf("%s = %d\n", var_name, value);
        } else {
            printf("Error: Variable '%s' not found\n", var_name);
        }
        free(line);
        return status;
    }

    char *equals = strchr(start, '=');
    if (!equals) {
        free(line);
        return ERROR_INVALID_INSTRUCTION;
    }

    *equals = '\0';
    char *var_name = start;
    char *expression = equals + 1;

    while (isspace(*var_name)) var_name++;
    end = var_name + strlen(var_name) - 1;
    while (end > var_name && isspace(*end)) *end-- = '\0';

    while (isspace(*expression)) expression++;
    end = expression + strlen(expression) - 1;
    while (end > expression && isspace(*end)) *end-- = '\0';

    int result;
    StatusCode status;

    char *expr_ptr = expression;
    if (*expr_ptr == '-' || *expr_ptr == '+') {
        expr_ptr++;
    }
    char *operator = strpbrk(expr_ptr, "+-*/%");

    if (operator) {
        char op = *operator;
        *operator = '\0';
        char *left_str = expression;
        char *right_str = operator + 1;

        while (isspace(*right_str)) right_str++;
        end = right_str + strlen(right_str) - 1;
        while (end > right_str && isspace(*end)) *end-- = '\0';

        status = process_arithmetic(left_str, right_str, op, &result, interpreter);
    } else {
        if (*expression == '-' || *expression == '+' || isdigit(*expression)) {
            status = safe_strtol(expression, &result);
        } else {
            status = get_variable_value(interpreter, expression, &result);
        }
    }

    if (status != SUCCESS) {
        free(line);
        return status;
    }

    status = add_variable(interpreter, var_name, result);
    free(line);
    return status;
}

void interpreter_cleanup(Interpreter *interpreter) {
    for (size_t i = 0; i < interpreter->size; i++) {
        free(interpreter->cells[i].name);
    }
    free(interpreter->cells);
    interpreter->cells = NULL;
    interpreter->size = 0;
    interpreter->capacity = 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return ERROR_FILE_OPEN;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        printf("Error: Cannot open file '%s'\n", argv[1]);
        return ERROR_FILE_READ;
    }

    Interpreter interpreter;
    if (interpreter_init(&interpreter) != SUCCESS) {
        fclose(file);
        printf("Error: Failed to initialize interpreter\n");
        return ERROR_MEMORY_ALLOCATION;
    }

    char line[256];
    int line_number = 0;
    StatusCode final_status = SUCCESS;

    while (fgets(line, sizeof(line), file)) {
        line_number++;
        if (strspn(line, " \t\n\r") == strlen(line)) {
            continue;
        }

        StatusCode status = process_instruction(&interpreter, line);
        if (status != SUCCESS) {
            printf("Error at line %d: ", line_number);
            switch (status) {
                case ERROR_MEMORY_ALLOCATION:
                    printf("Memory allocation failed\n");
                    break;
                case ERROR_INVALID_INSTRUCTION:
                    printf("Invalid instruction\n");
                    break;
                case ERROR_VARIABLE_NOT_FOUND:
                    printf("Variable not found\n");
                    break;
                case ERROR_DIVISION_BY_ZERO:
                    printf("Division by zero\n");
                    break;
                case ERROR_NUMBER_OVERFLOW:
                    printf("Number overflow\n");
                    break;
                case ERROR_INVALID_NUMBER:
                    printf("Invalid number format\n");
                    break;
                default:
                    printf("Unknown error (code %d)\n", status);
            }
            final_status = status;
            break;
        }
    }

    fclose(file);
    interpreter_cleanup(&interpreter);

    return (int) final_status;
}
