#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#define INITIAL_HASHSIZE 128
#define MAX_MACRO_LENGTH 1024
#define MIN_HASHSIZE 64
#define MAX_HASHSIZE 1024

typedef struct MacroNode {
    char* name;
    char* value;
    struct MacroNode* next;
} MacroNode;

typedef struct {
    MacroNode** table;
    size_t size;
    size_t count;
} HashTable;

typedef enum {
    SUCCESS = 0,
    ERROR_MEMORY_ALLOCATION,
    ERROR_FILE_OPERATION,
    ERROR_INVALID_ARGS,
    ERROR_INVALID_DEFINE,
    ERROR_HASH_TABLE_FULL
} StatusCode;

static unsigned long hash_function(const char* str, size_t table_size) {
    unsigned long hash = 0;
    int c;

    while ((c = *str++)) {
        if (isdigit(c)) {
            hash = hash * 62 + (c - '0');
        } else if (isupper(c)) {
            hash = hash * 62 + (c - 'A' + 10);
        } else if (islower(c)) {
            hash = hash * 62 + (c - 'a' + 36);
        }
    }

    return hash % table_size;
}

StatusCode create_hash_table(HashTable* ht, size_t size) {
    ht->table = calloc(size, sizeof(MacroNode*));
    if (!ht->table) {
        return ERROR_MEMORY_ALLOCATION;
    }

    ht->size = size;
    ht->count = 0;
    return SUCCESS;
}

void free_hash_table(HashTable* ht) {
    for (size_t i = 0; i < ht->size; i++) {
        MacroNode* current = ht->table[i];
        while (current) {
            MacroNode* next = current->next;
            free(current->name);
            free(current->value);
            free(current);
            current = next;
        }
    }
    free(ht->table);
    ht->table = NULL;
    ht->size = 0;
    ht->count = 0;
}

StatusCode insert_macro(HashTable* ht, const char* name, const char* value) {
    unsigned long hash = hash_function(name, ht->size);

    MacroNode* current = ht->table[hash];
    while (current) {
        if (strcmp(current->name, name) == 0) {
            char* new_value = strdup(value);
            if (!new_value) {
                return ERROR_MEMORY_ALLOCATION;
            }
            free(current->value);
            current->value = new_value;
            return SUCCESS;
        }
        current = current->next;
    }

    MacroNode* new_node = malloc(sizeof(MacroNode));
    if (!new_node) {
        return ERROR_MEMORY_ALLOCATION;
    }

    new_node->name = strdup(name);
    new_node->value = strdup(value);
    if (!new_node->name || !new_node->value) {
        free(new_node->name);
        free(new_node->value);
        free(new_node);
        return ERROR_MEMORY_ALLOCATION;
    }

    new_node->next = ht->table[hash];
    ht->table[hash] = new_node;
    ht->count++;

    return SUCCESS;
}

const char* find_macro(const HashTable* ht, const char* name) {
    unsigned long hash = hash_function(name, ht->size);
    MacroNode* current = ht->table[hash];

    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current->value;
        }
        current = current->next;
    }

    return NULL;
}

StatusCode rebalance_if_needed(HashTable* ht) {
    size_t min_chain = SIZE_MAX;
    size_t max_chain = 0;

    for (size_t i = 0; i < ht->size; i++) {
        size_t chain_length = 0;
        MacroNode* current = ht->table[i];
        while (current) {
            chain_length++;
            current = current->next;
        }
        if (chain_length < min_chain) min_chain = chain_length;
        if (chain_length > max_chain) max_chain = chain_length;
    }

    if (max_chain >= 2 && min_chain * 2 <= max_chain) {
        size_t new_size;

        if (max_chain > ht->count / 2) {
            new_size = ht->size * 2;
        } else {
            new_size = ht->size / 2;
        }

        if (new_size < MIN_HASHSIZE) new_size = MIN_HASHSIZE;
        if (new_size > MAX_HASHSIZE) new_size = MAX_HASHSIZE;

        HashTable new_ht;
        StatusCode status = create_hash_table(&new_ht, new_size);
        if (status != SUCCESS) {
            return status;
        }

        for (size_t i = 0; i < ht->size; i++) {
            MacroNode* current = ht->table[i];
            while (current) {
                status = insert_macro(&new_ht, current->name, current->value);
                if (status != SUCCESS) {
                    free_hash_table(&new_ht);
                    return status;
                }
                current = current->next;
            }
        }

        free_hash_table(ht);
        *ht = new_ht;
    }

    return SUCCESS;
}

StatusCode process_define(char* line, HashTable* ht) {
    char name[MAX_MACRO_LENGTH] = {0};
    char value[MAX_MACRO_LENGTH] = {0};

    char* start = line + 7;  // Skip "#define"
    while (*start && isspace(*start)) start++;

    char* ptr = name;
    while (*start && !isspace(*start)) {
        if (ptr - name >= MAX_MACRO_LENGTH - 1) {
            return ERROR_INVALID_DEFINE;
        }
        *ptr++ = *start++;
    }
    *ptr = '\0';

    while (*start && isspace(*start)) start++;

    ptr = value;
    while (*start && *start != '\n' && *start != '\r') {
        if (ptr - value >= MAX_MACRO_LENGTH - 1) {
            return ERROR_INVALID_DEFINE;
        }
        *ptr++ = *start++;
    }
    *ptr = '\0';

    // Удаляем конечные пробелы из value
    while (ptr > value && isspace(*(ptr-1))) {
        *(--ptr) = '\0';
    }

    if (name[0] == '\0' || value[0] == '\0') {
        return ERROR_INVALID_DEFINE;
    }

    return insert_macro(ht, name, value);
}

StatusCode process_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return ERROR_FILE_OPERATION;
    }

    HashTable ht;
    StatusCode status = create_hash_table(&ht, INITIAL_HASHSIZE);
    if (status != SUCCESS) {
        fclose(file);
        return status;
    }

    char line[MAX_MACRO_LENGTH];

    // First pass: process all #define directives
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "#define", 7) == 0) {
            status = process_define(line, &ht);
            if (status != SUCCESS) {
                free_hash_table(&ht);
                fclose(file);
                return status;
            }

            status = rebalance_if_needed(&ht);
            if (status != SUCCESS) {
                free_hash_table(&ht);
                fclose(file);
                return status;
            }
        }
    }

    // Reset file pointer
    rewind(file);

    // Second pass: process the text
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "#define", 7) == 0) {
            continue;
        }

        size_t line_len = strlen(line);
        char output[MAX_MACRO_LENGTH * 2] = {0};  // Увеличенный буфер для выходной строки
        size_t out_pos = 0;
        size_t pos = 0;

        while (pos < line_len) {
            if (isalnum(line[pos]) || line[pos] == '_') {
                char word[MAX_MACRO_LENGTH] = {0};
                size_t word_pos = 0;

                while (pos < line_len && (isalnum(line[pos]) || line[pos] == '_')) {
                    word[word_pos++] = line[pos++];
                }

                const char* replacement = find_macro(&ht, word);
                if (replacement) {
                    strcpy(output + out_pos, replacement);
                    out_pos += strlen(replacement);
                } else {
                    strcpy(output + out_pos, word);
                    out_pos += strlen(word);
                }
            } else {
                output[out_pos++] = line[pos++];
            }
        }

        printf("%s", output);
    }

    free_hash_table(&ht);
    fclose(file);
    return SUCCESS;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return ERROR_INVALID_ARGS;
    }

    StatusCode status = process_file(argv[1]);
    if (status != SUCCESS) {
        const char* error_messages[] = {
                "Success",
                "Memory allocation error",
                "File operation error",
                "Invalid arguments",
                "Invalid #define directive",
                "Hash table full"
        };
        fprintf(stderr, "Error: %s\n", error_messages[status]);
        return status;
    }

    return SUCCESS;
}
