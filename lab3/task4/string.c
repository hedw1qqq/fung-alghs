#include "string.h"


errors create_str(String *arr, char *str) {
    arr->length = strlen(str);
    arr->capacity = arr->length + 5;
    arr->val = (char *) malloc(sizeof(char) * arr->capacity);

    if (!arr->val) return memory_error;

    int i = 0;

    for (i = 0; i < arr->length; ++i) {
        arr->val[i] = str[i];
    }

    arr->val[i] = '\0';

    return ok;
}

errors append_str(String *arr, char value) {
    if (arr->length >= (arr->capacity - 2)) {
        int err = extend_str(arr);
        if (err)
            return err;
    }

    arr->val[arr->length++] = value;
    arr->val[arr->length] = '\0';
    return ok;
}

void reset_str(String *arr) {
    arr->val[0] = '\0';
    arr->length = 0;
}

errors resize_str(String *arr, int size_delta) {
    char *new_addr = (char *) realloc(arr->val, arr->capacity + size_delta * sizeof(char));

    if (!new_addr) {
        free(arr->val);
        return memory_error;
    }

    arr->val = new_addr;
    arr->capacity += size_delta;
    return ok;
}

int extend_str(String *arr) {
    return resize_str(arr, arr->capacity);
}

void destroy_str(String *arr) {
    if (arr->val && arr!=NULL) {
        free(arr->val);
        arr->val = NULL;
    }
    arr->length = 0;
    arr->capacity = 0;
}

errors copystr(String *dst, String *src) {
    destroy_str(dst);

    dst->val = (char *) calloc(src->capacity, sizeof(char));
    if (!dst->val)
        return memory_error;

    dst->length = src->length;
    dst->capacity = src->capacity;

    char *tempdst = dst->val, *tempsrc = src->val;
    while ((*dst->val++ = *src->val++));
    dst->val = tempdst;
    src->val = tempsrc;
    return ok;
}

errors copy_newstr(String *dst, const String *src) {
    int err = create_str(dst, src->val);
    if (err)
        return err;

    return ok;
}


int compare_str(String a, String b) {
    if (a.length > b.length)
        return 1;
    else if (a.length < b.length)
        return -1;

    char *str1 = a.val;
    char *str2 = b.val;

    while (*str1 && *str2) {
        if (*str1 < *str2) return -1;
        if (*str1 > *str2) return 1;
        str1++;
        str2++;
    }
    if (*str1) return 1;
    if (*str2) return -1;
    return 0;
}

int equiv_str(String a, String b) {
    if (compare_str(a, b) == 0)
        return 1;
    return 0;
}