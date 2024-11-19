#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <limits.h>

#define MAX_ARRAYS 26
#define MAX_COMMAND_LENGTH 256
#define INITIAL_ARRAY_CAPACITY 10

typedef enum {
    OK,
    ERROR_MEMORY,
    ERROR_INVALID_INDEX,
    ERROR_FILE_OPEN,
    ERROR_FILE_READ,
    ERROR_FILE_WRITE,
    ERROR_INVALID_COMMAND,
    ERROR_INVALID_ARRAY_NAME,
    ERROR_INVALID_PARAMS,
    ERROR_ARRAY_NOT_FOUND
} StatusCode;

typedef struct {
    int *data;
    int size;
    int capacity;
} Array;

typedef struct {
    Array arrays[MAX_ARRAYS];
    int initialized[MAX_ARRAYS];
} ArrayStorage;

StatusCode ensure_capacity(Array *array, int needed_size) {
    if (needed_size <= array->capacity) return OK;

    int new_capacity = array->capacity;
    while (new_capacity < needed_size) {
        new_capacity *= 2;
    }

    int *new_data = realloc(array->data, new_capacity * sizeof(int));
    if (!new_data) return ERROR_MEMORY;

    array->data = new_data;
    array->capacity = new_capacity;
    return OK;
}

int get_array_index(char name) {
    if (!isalpha(name)) {
        return -1;
    }

    name = toupper(name);
    if (name < 'A' || name > 'Z') {
        return -1;
    }

    return name - 'A';
}

StatusCode init_array(Array *array) {
    array->data = malloc(INITIAL_ARRAY_CAPACITY * sizeof(int));
    if (!array->data) return ERROR_MEMORY;

    array->size = 0;
    array->capacity = INITIAL_ARRAY_CAPACITY;
    return OK;
}

void free_array(Array *array) {
    if (array) {
        free(array->data);
        array->data = NULL;
        array->size = 0;
        array->capacity = 0;
    }
}

StatusCode cmd_load(Array *array, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return ERROR_FILE_OPEN;

    array->size = 0;
    char word[256];

    while (!feof(file)) {

        if (fscanf(file, "%255s", word) != 1) {
            break;
        }

        char *endptr;
        long value = strtol(word, &endptr, 10);

        if (*endptr != '\0' || value > INT_MAX || value < INT_MIN) {
            continue;
        }

        StatusCode status = ensure_capacity(array, array->size + 1);
        if (status != OK) {
            fclose(file);
            return status;
        }

        array->data[array->size++] = (int) value;
    }

    fclose(file);
    return OK;
}

StatusCode cmd_save(const Array *array, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) return ERROR_FILE_OPEN;

    for (int i = 0; i < array->size; i++) {
        if (fprintf(file, "%d ", array->data[i]) < 0) {
            fclose(file);
            return ERROR_FILE_WRITE;
        }
    }

    fclose(file);
    return OK;
}

StatusCode cmd_rand(Array *array, int count, int lb, int rb) {
    if (count < 0 || lb > rb) return ERROR_INVALID_PARAMS;

    StatusCode status = ensure_capacity(array, count);
    if (status != OK) return status;

    array->size = count;
    for (int i = 0; i < count; i++) {
        array->data[i] = lb + rand() % (rb - lb + 1);
    }

    return OK;
}

int compare_asc(const void *a, const void *b) {
    return (*(int *) a - *(int *) b);
}

int compare_desc(const void *a, const void *b) {
    return (*(int *) b - *(int *) a);
}

StatusCode cmd_sort(Array *array, int ascending) {
    if (array->size == 0) return OK;
    qsort(array->data, array->size, sizeof(int), ascending ? compare_asc : compare_desc);
    return OK;
}

StatusCode cmd_shuffle(Array *array) {
    for (int i = array->size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array->data[i];
        array->data[i] = array->data[j];
        array->data[j] = temp;
    }
    return OK;
}

void find_min_max_mean(const Array *array, int *min, int *min_idx, int *max, int *max_idx, double *mean) {
    *min = array->data[0];
    *max = array->data[0];
    *min_idx = 0;
    *max_idx = 0;

    double sum = array->data[0];

    for (int i = 1; i < array->size; i++) {
        if (array->data[i] < *min) {
            *min = array->data[i];
            *min_idx = i;
        }
        if (array->data[i] > *max) {
            *max = array->data[i];
            *max_idx = i;
        }
        sum += array->data[i];
    }
    *mean = sum / array->size;
}


void find_deviation(const Array *array, double *max_deviation, const double mean) {
    *max_deviation = 0;
    for (int i = 0; i < array->size; i++) {
        double deviation = fabs(array->data[i] - mean);
        if (deviation > *max_deviation) {
            *max_deviation = deviation;
        }
    }
}

void find_most_frequent(const Array *array, int *most_freq, int *frequency) {
    if (array->size == 0) {
        *most_freq = 0;
        *frequency = 0;
        return;
    }


    Array temp_array = *array;
    qsort(temp_array.data, temp_array.size, sizeof(int), compare_asc);


    int current_freq = 1;
    int max_freq = 1;
    int current_element = temp_array.data[0];
    *most_freq = temp_array.data[0];

    for (int i = 1; i < temp_array.size; i++) {
        if (temp_array.data[i] == current_element) {
            current_freq++;
        } else {
            if (current_freq > max_freq ||
                (current_freq == max_freq && current_element > *most_freq)) {
                max_freq = current_freq;
                *most_freq = current_element;
            }
            current_freq = 1;
            current_element = temp_array.data[i];
        }
    }

    if (current_freq > max_freq ||
        (current_freq == max_freq && current_element > *most_freq)) {
        max_freq = current_freq;
        *most_freq = current_element;
    }

    *frequency = max_freq;
}

StatusCode cmd_stats(const Array *array) {
    if (array->size == 0) {
        printf("Array is empty\n");
        return OK;
    }

    int min, max, min_idx, max_idx, most_freq, frequency;
    double mean, max_deviation;

    find_min_max_mean(array, &min, &min_idx, &max, &max_idx, &mean);
    find_deviation(array, &max_deviation, mean);
    find_most_frequent(array, &most_freq, &frequency);

    printf("Size: %d\n", array->size);
    printf("Min: %d (index: %d)\n", min, min_idx);
    printf("Max: %d (index: %d)\n", max, max_idx);
    printf("Most frequent: %d (frequency: %d)\n", most_freq, frequency);
    printf("Mean: %.2f\n", mean);
    printf("Max deviation: %.2f\n", max_deviation);

    return OK;
}

StatusCode cmd_print(const Array *array, int start, int end) {
    if (start < 0 || start >= array->size || end < start || end >= array->size) {
        return ERROR_INVALID_INDEX;
    }

    for (int i = start; i <= end; i++) {
        printf("%d ", array->data[i]);
    }
    printf("\n");
    return OK;
}

StatusCode process_command(ArrayStorage *storage, char *command) {
    char cmd[MAX_COMMAND_LENGTH];
    char array_name;

    if (sscanf(command, "%s %c", cmd, &array_name) != 2) {
        return ERROR_INVALID_COMMAND;
    }

    int array_idx = get_array_index(array_name);
    if (array_idx < 0) return ERROR_INVALID_ARRAY_NAME;

    Array *array = &storage->arrays[array_idx];

    for (char *p = cmd; *p; p++) {
        *p = tolower(*p);
    }

    if (strcmp(cmd, "load") == 0) {
        char filename[256];
        if (sscanf(command, "%*s %*c , %255s", filename) != 1) {
            return ERROR_INVALID_PARAMS;
        }
        return cmd_load(array, filename);
    }

    if (strcmp(cmd, "save") == 0) {
        char filename[256];
        if (sscanf(command, "%*s %*c , %255s", filename) != 1) {
            return ERROR_INVALID_PARAMS;
        }
        return cmd_save(array, filename);
    }

    if (strcmp(cmd, "rand") == 0) {
        int count, lb, rb;
        if (sscanf(command, "%*s %*c , %d , %d , %d", &count, &lb, &rb) != 3) {
            return ERROR_INVALID_PARAMS;
        }
        return cmd_rand(array, count, lb, rb);
    }

    if (strcmp(cmd, "concat") == 0) {
        char second_array_name;
        if (sscanf(command, "%*s %*c , %c", &second_array_name) != 1) {
            return ERROR_INVALID_PARAMS;
        }

        int second_idx = get_array_index(second_array_name);
        if (second_idx < 0) return ERROR_INVALID_ARRAY_NAME;

        Array *second_array = &storage->arrays[second_idx];

        StatusCode status = ensure_capacity(array, array->size + second_array->size);
        if (status != OK) return status;

        memcpy(array->data + array->size, second_array->data, second_array->size * sizeof(int));
        array->size += second_array->size;

        return OK;
    }

    if (strcmp(cmd, "free") == 0) {
        free_array(array);
        return init_array(array);
    }

    if (strcmp(cmd, "remove") == 0) {
        int start, count;
        if (sscanf(command, "%*s %*c , %d , %d", &start, &count) != 2) {
            return ERROR_INVALID_PARAMS;
        }

        if (start < 0 || start >= array->size || count < 0 || start + count > array->size) {
            return ERROR_INVALID_INDEX;
        }

        memmove(array->data + start,
                array->data + start + count,
                (array->size - start - count) * sizeof(int));
        array->size -= count;

        return OK;
    }

    if (strcmp(cmd, "copy") == 0) {
        int start, end;
        char dest_array_name;
        if (sscanf(command, "%*s %*c , %d , %d , %c", &start, &end, &dest_array_name) != 3) {
            return ERROR_INVALID_PARAMS;
        }

        if (start < 0 || end >= array->size || start > end) {
            return ERROR_INVALID_INDEX;
        }

        int dest_idx = get_array_index(dest_array_name);
        if (dest_idx < 0) return ERROR_INVALID_ARRAY_NAME;

        Array *dest_array = &storage->arrays[dest_idx];
        int count = end - start + 1;

        StatusCode status = ensure_capacity(dest_array, count);
        if (status != OK) return status;

        memcpy(dest_array->data, array->data + start, count * sizeof(int));
        dest_array->size = count;

        return OK;
    }

    if (strcmp(cmd, "sort") == 0) {
        char direction;
        if (sscanf(command, "%*s %*c%c", &direction) != 1) {
            return ERROR_INVALID_PARAMS;
        }
        return cmd_sort(array, direction == '+');
    }

    if (strcmp(cmd, "shuffle") == 0) {
        return cmd_shuffle(array);
    }

    if (strcmp(cmd, "stats") == 0) {
        return cmd_stats(array);
    }

    if (strcmp(cmd, "print") == 0) {
        char param[10];
        if (sscanf(command, "%*s %*c , %s", param) == 1) {
            if (strcmp(param, "all") == 0) {
                return cmd_print(array, 0, array->size - 1);
            }
            int start, end;
            if (sscanf(command, "%*s %*c , %d , %d", &start, &end) == 2) {
                return cmd_print(array, start, end);
            }
            int index;
            if (sscanf(param, "%d", &index) == 1) {
                if (index < 0 || index >= array->size) {
                    return ERROR_INVALID_INDEX;
                }
                printf("%d\n", array->data[index]);
                return OK;
            }
        }


        return ERROR_INVALID_PARAMS;
    }

    return ERROR_INVALID_COMMAND;
}

int main() {
    ArrayStorage storage = {0};
    srand(time(NULL));

    for (int i = 0; i < MAX_ARRAYS; i++) {
        StatusCode status = init_array(&storage.arrays[i]);
        if (status != OK) {
            printf("Failed to initialize arrays\n");
            for (int j = 0; j < i; j++) {
                free_array(&storage.arrays[j]);
            }
            return 1;
        }
        storage.initialized[i] = 1;
    }

    char command[MAX_COMMAND_LENGTH];
    while (1) {
        printf("> ");
        if (!fgets(command, sizeof(command), stdin)) break;

        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "exit") == 0) break;

        StatusCode status = process_command(&storage, command);
        switch (status) {
            case OK:
                break;
            case ERROR_MEMORY:
                printf("Memory allocation error\n");
                break;
            case ERROR_INVALID_INDEX:
                printf("Invalid index\n");
                break;
            case ERROR_FILE_OPEN:
                printf("Cannot open file\n");
                break;
            case ERROR_FILE_READ:
                printf("File read error\n");
                break;
            case ERROR_FILE_WRITE:
                printf("File write error\n");
                break;
            case ERROR_INVALID_COMMAND:
                printf("Invalid command\n");
                break;
            case ERROR_INVALID_ARRAY_NAME:
                printf("Invalid array name\n");
                break;
            case ERROR_INVALID_PARAMS:
                printf("Invalid parameters\n");
                break;
            case ERROR_ARRAY_NOT_FOUND:
                printf("Array not found\n");
                break;
            default:
                printf("Unknown Error\n");
                break;
        }

    }

    for (int i = 0; i < MAX_ARRAYS; i++) {
        if (storage.initialized[i]) {
            free_array(&storage.arrays[i]);
        }
    }

    return 0;
}
