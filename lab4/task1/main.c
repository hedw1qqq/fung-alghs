#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_HASHSIZE 128
#define REHASH_THRESHOLD 2

typedef struct Macro {
    char *name;
    char *value;
    struct Macro *next;
} Macro;

typedef struct {
    Macro **table;
    int size;
    int numElements;
} HashTable;


unsigned long hashFunction(const char *name, int hashSize) {
    unsigned long hash = 0;
    for (int i = 0; name[i] != '\0'; i++) {
        int charValue;
        if (isdigit(name[i])) {
            charValue = name[i] - '0';
        } else if (isupper(name[i])) {
            charValue = name[i] - 'A' + 10;
        } else {
            charValue = name[i] - 'a' + 36;
        }
        hash = hash * 62 + charValue;
    }
    return hash % hashSize;
}


HashTable* createHashTable(int size) {
    HashTable *table = (HashTable*)malloc(sizeof(HashTable));
    table->table = (Macro**)calloc(size, sizeof(Macro*));
    table->size = size;
    table->numElements = 0;
    return table;
}

Macro *createMacro(const char *name, const char *value) {
    Macro *newMacro = (Macro*)malloc(sizeof(Macro));
    if (!newMacro) {
        fprintf(stderr, "Ошибка выделения памяти для Macro\n");
        exit(EXIT_FAILURE);
    }
    newMacro->name = (char*)malloc(strlen(name) + 1);
    if (!newMacro->name) {
        fprintf(stderr, "Ошибка выделения памяти для имени Macro\n");
        free(newMacro);
        exit(EXIT_FAILURE);
    }
    strcpy(newMacro->name, name);

    newMacro->value = (char*)malloc(strlen(value) + 1);
    if (!newMacro->value) {
        fprintf(stderr, "Ошибка выделения памяти для значения Macro\n");
        free(newMacro->name);
        free(newMacro);
        exit(EXIT_FAILURE);
    }
    strcpy(newMacro->value, value);

    newMacro->next = NULL;
    return newMacro;
}

void freeMacro(Macro *macro) {
    if (macro) {
        free(macro->name);
        free(macro->value);
        free(macro);
    }
}

void insertMacro(HashTable *table, const char *name, const char *value) {
    unsigned long hashIndex = hashFunction(name, table->size);
    Macro *newMacro = createMacro(name, value);
    newMacro->next = table->table[hashIndex];
    table->table[hashIndex] = newMacro;
    table->numElements++;
}

char* findMacro(HashTable *table, const char *name) {
    unsigned long hashIndex = hashFunction(name, table->size);
    Macro *entry = table->table[hashIndex];
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

void rehash(HashTable *table) {
    int newSize = table->size * 2;
    HashTable *newTable = createHashTable(newSize);
    for (int i = 0; i < table->size; i++) {
        Macro *entry = table->table[i];
        while (entry) {
            insertMacro(newTable, entry->name, entry->value);
            entry = entry->next;
        }
    }
    free(table->table);
    table->table = newTable->table;
    table->size = newSize;
}

void parseDefines(HashTable *table, FILE *file) {
    char line[512];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "#define", 7) == 0) {
            char name[128], value[256];
            sscanf(line + 7, "%s %[^\n]", name, value);
            insertMacro(table, name, value);
        } else {
            break;
        }
    }
}

void replaceMacros(HashTable *table, FILE *input, FILE *output) {
    char word[128];
    int c;
    while ((c = fgetc(input)) != EOF) {
        if (isalnum(c) || c == '_') {
            int len = 0;
            word[len++] = c;
            while (isalnum(c = fgetc(input)) || c == '_') {
                if (len < sizeof(word) - 1) {
                    word[len++] = c;
                }
            }
            word[len] = '\0';
            char *value = findMacro(table, word);
            if (value) {
                fputs(value, output);
            } else {
                fputs(word, output);
            }
            if (c != EOF) {
                fputc(c, output);
            }
        } else {
            fputc(c, output);
        }
    }
}

void freeHashTable(HashTable *table) {
    for (int i = 0; i < table->size; i++) {
        Macro *entry = table->table[i];
        while (entry) {
            Macro *temp = entry;
            entry = entry->next;
            freeMacro(temp);
        }
    }
    free(table->table);
    free(table);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    HashTable *table = createHashTable(INITIAL_HASHSIZE);
    parseDefines(table, file);
    replaceMacros(table, file, stdout);
    freeHashTable(table);
    fclose(file);
    return 0;
}
