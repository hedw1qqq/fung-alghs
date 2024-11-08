#ifndef LAB3_STRING_H
#define LAB3_STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef enum errors {
    ok,
    memory_error,

} errors;

typedef struct String {
    char *val;
    unsigned int length;
    unsigned int capacity;
} String;

errors create_str(String *arr, char *str);

errors append_str(String *arr, char value);

void reset_str(String *arr);

errors resize_str(String *arr, int size_delta);

int extend_str(String *arr);

void destroy_str(String *arr);

errors copystr(String *dst, String *src);

errors copy_newstr(String *dst, const String *src);

int concat_str(String *A, String B);

int compare_str(String a, String b);

int equiv_str(String a, String b);

#endif