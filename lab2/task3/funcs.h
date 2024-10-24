//
// Created by ivglu on 21.10.2024.
//

#ifndef TASK3_FUNCS_H
#define TASK3_FUNCS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

typedef enum {
    OK,
    FILE_OPEN_ERROR,
    MEMORY_ALLOCATION_ERROR,
    INVALID_ARGUMENTS
} StatusCode;

struct Sub {
    char *filename;
    int line;
    int idx;
    struct Sub *next;
};

int pos_in_line(const char *str, int size);

int substr(const char *str, int n, const char *key, int m);

StatusCode add_result(struct Sub **head, const char *filename, int line, int idx);

int line_counter(const char *str, int size);

int file_handler(char *key, int m, char *filename, struct Sub **result_list);

int find_substr_in_files(char *substr, int m, int file_count, struct Sub **result_list, ...);

void free_result_list(struct Sub *head);

#endif //TASK3_FUNCS_H
