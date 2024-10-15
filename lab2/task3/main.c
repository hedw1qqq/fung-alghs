#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

typedef struct {
    int line_number;
    int char_position;
} SubstringOccurrence;

int find_substring(const char *string, const char *substring) {
    int i = 0, j = 0;
    while (string[i] != '\0') {
        // Проверяем начало возможного совпадения
        while (string[i + j] != '\0' && substring[j] != '\0' && string[i + j] == substring[j]) {
            j++;
        }
        // Если подстрока полностью совпала
        if (substring[j] == '\0') {
            return i;
        }
        // Переходим к следующему символу строки
        i++;
        j = 0;
    }
    // Подстрока не найдена
    return -1;
}

int search_in_files(const char *substring, SubstringOccurrence **occurrences, int num_files, ...) {
    va_list args;
    va_start(args, num_files);

    const int BUFFER_SIZE = 1024;  // Размер буфера для строк
    int substring_len = strlen(substring);  // Длина подстроки
    int total_occurrences = 0;  // Общее количество найденных вхождений

    *occurrences = NULL;  // Инициализируем указатель на массив результатов

    for (int i = 0; i < num_files; i++) {
        const char *file_path = va_arg(args, const char*);

        FILE *file = fopen(file_path, "r");
        if (!file) {
            printf("Error opening file: %s\n", file_path);
            continue;
        }

        char line[BUFFER_SIZE];
        char prev_part[BUFFER_SIZE];  // Храним часть предыдущей строки
        int line_number = 0;

        while (fgets(line, sizeof(line), file)) {
            line_number++;

            // Если есть остаток от предыдущей строки, объединяем его с текущей
            char combined_line[BUFFER_SIZE * 2];
            strcpy(combined_line, prev_part); // Копируем остаток предыдущей строки
            strcat(combined_line, line);      // Конкатенируем с текущей строкой

            int pos = 0;

            // Поиск вхождений подстроки в объединенной строке
            while ((pos = find_substring(&combined_line[pos], substring)) != -1) {
                // Выделяем память для хранения нового результата
                total_occurrences++;
                *occurrences = (SubstringOccurrence *) realloc(*occurrences, total_occurrences * sizeof(SubstringOccurrence));
                if (*occurrences == NULL) {
                    fclose(file);
                    va_end(args);
                    return -1;  // Ошибка при выделении памяти
                }

                // Заполняем информацию о найденном вхождении
                (*occurrences)[total_occurrences - 1].line_number = line_number - 1;  // Номер строки
                (*occurrences)[total_occurrences - 1].char_position = pos + 1;        // Позиция символа

                pos++;  // Переходим на следующий символ после найденного вхождения
            }

            // Обновляем остаток строки для следующего чтения
            if (strlen(line) >= substring_len) {
                strncpy(prev_part, &line[strlen(line) - substring_len + 1], substring_len - 1);
                prev_part[substring_len - 1] = '\0';
            } else {
                strcpy(prev_part, line);
            }
        }
        fclose(file);
    }

    va_end(args);
    return total_occurrences;
}

int main() {
    SubstringOccurrence *occurrences = NULL;
    int num_occurrences = search_in_files("AAAA", &occurrences, 2, "1.txt", "2.txt");

    if (num_occurrences > 0) {
        for (int i = 0; i < num_occurrences; i++) {
            printf("Found at line %d, position %d\n",
                   occurrences[i].line_number, occurrences[i].char_position);
        }
    } else {
        printf("No occurrences found.\n");
    }

    // Освобождаем память
    free(occurrences);

    return 0;
}
