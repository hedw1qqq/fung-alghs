//
// Created by ivglu on 21.10.2024.
//

#include "funcs.h"

int pos_in_line(const char *str, int size) {
    if (str[size - 1] == '\n')
        return 1;
    for (int i = size - 2; i >= 0; --i) {
        if (str[i] == '\n')
            return size - i - 1;
    }
    return size;
}

int substr(const char *str, int n, const char *key, int m) {
    for (int i = 0; i <= n - m; ++i) {
        int j = 0;
        while (j < m && str[i + j] == key[j]) {
            j++;
        }
        if (j == m) {
            return i;
        }
    }
    return -1;
}

StatusCode add_result(struct Sub **head, const char *filename, int line, int idx) {
    struct Sub *new_node = malloc(sizeof(struct Sub));
    if (new_node == NULL) {
        return MEMORY_ALLOCATION_ERROR;
    }

    new_node->filename = strdup(filename);
    if (new_node->filename == NULL) {
        free(new_node);
        return MEMORY_ALLOCATION_ERROR;
    }

    new_node->line = line;
    new_node->idx = idx;
    new_node->next = *head;
    *head = new_node;

    return OK;
}

int line_counter(const char *str, int size) {
    int count = 1;
    for (int i = 0; i < size; ++i) {
        if (str[i] == '\n')
            count++;
    }
    return count;
}

int file_handler(char *key, int m, char *filename, struct Sub **result_list) {
    FILE *stream = fopen(filename, "r");
    if (!stream)
        return FILE_OPEN_ERROR;

    fseek(stream, 0, SEEK_END);
    int size = ftell(stream);
    fseek(stream, 0, SEEK_SET);

    char *content = malloc(size);
    if (!content) {
        fclose(stream);
        return MEMORY_ALLOCATION_ERROR;
    }

    fread(content, 1, size, stream);
    fclose(stream);

    int i = 0, ind = 0;

    while (1) {
        i = substr(content + ind, size - ind, key, m);
        if (i == -1) break;
        ind += i + 1;
        StatusCode status = add_result(result_list, filename, line_counter(content, ind), pos_in_line(content, ind));
        if (status != OK) {
            free(content);
            return status;
        }
    }

    free(content);
    return OK;
}

int find_substr_in_files(char *substr, int m, int file_count, struct Sub **result_list, ...) {
    va_list files;
    char *filename;

    va_start(files, result_list);
    for (int i = 0; i < file_count; ++i) {
        filename = va_arg(files, char*);
        int err = file_handler(substr, m, filename, result_list);
        if (err) {
            va_end(files);
            return err;
        }
    }
    va_end(files);

    return 0;
}

void free_result_list(struct Sub *head) {
    while (head != NULL) {
        struct Sub *temp = head;
        head = head->next;
        free(temp->filename);
        free(temp);
    }
}