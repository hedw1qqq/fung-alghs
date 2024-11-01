//
// Created by ivglu on 01.11.2024.
//

#ifndef LAB3_STRING_H
#define LAB3_STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


typedef struct {
    char *data;
    int length;
} String;
typedef enum {
    ok = 0,
    memory_error,
    invalid_data,
} errors;

errors create_string(String *s, const char *input);


errors delete_string(String *s);


errors compare_strings(const String *a, const String *b, int *result);


errors equals_strings(const String *a, const String *b, int *result);


errors copy_string(String *dest, const String *src);


errors copy_to_new_string(const String *src, String *dest);


errors concat_strings(String *a, const String *b);

const char *get_error_message(errors err);

#endif //LAB3_STRING_H
