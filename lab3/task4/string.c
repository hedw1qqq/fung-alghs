//
// Created by ivglu on 01.11.2024.
//

#include "string.h"

errors create_string(String *s, const char *input) {
    if (!s || !input) {
        return invalid_data;
    }
    s->data = strdup(input);
    if (!s->data) {
        return memory_error;
    }
    s->length = strlen(input);
    return ok;
}

errors delete_string(String *s) {
    if (!s) {
        return invalid_data;

    }
    if (s->data) {
        free(s->data);
        s->data = NULL;
        s->length = 0;
    }
    return ok;
}

errors compare_strings(const String *a, const String *b, int *result) {
    if (!a || !b) {
        return invalid_data;

    }
    if (a->length != b->length) {
        *result = (a->length < b->length) ? -1 : 1;
    } else {
        *result = strcmp(a->data, b->data);
    }
    return ok;
}


errors equals_strings(const String *a, const String *b, int *result) {
    if (!a || !b || !result){
        return invalid_data;
    }
    *result = (strcmp(a->data, b->data) == 0);
    return ok;
}


errors copy_string(String *dest, const String *src) {
    if (!dest || !src) {
        return invalid_data;

    }
    char *new_data = strdup(src->data);
    if (!new_data) {
        return memory_error;
    }
    delete_string(dest);
    dest->data = new_data;
    dest->length = src->length;
    return ok;
}


errors copy_to_new_string(const String *src, String *dest) {
    if (!src || !dest) {
        return invalid_data;
    }
    return create_string(dest, src->data);
}


errors concat_strings(String *a, const String *b) {
    if (!a || !b || !a->data || !b->data) {
        return invalid_data;
    }


    char *new_data = realloc(a->data, a->length + b->length + 1);
    if (!new_data) {
        return memory_error;
    }

    a->data = new_data;

    memcpy(a->data + a->length, b->data, b->length);
    a->data[a->length + b->length] = '\0';
    a->length += b->length;

    return ok;
}

const char *get_error_message(errors err) {
    switch (err) {
        case ok:
            return "No error";
        case memory_error:
            return "Memory allocation failed";
        case invalid_data:
            return "Invalid data";
        default:
            return "Unknown error";
    }
}