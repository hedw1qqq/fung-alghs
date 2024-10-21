#include "funcs.h"
// Пример работы программы
int main() {
    struct Sub *result_list = NULL;
    char *substr = "q\n         n";  // Подстрока для поиска
    int substr_len = strlen(substr);

    StatusCode status = find_substr_in_files(substr, substr_len, 2, &result_list, "1.txt", "2.txt");

    switch (status) {
        case OK:
            printf("Search completed successfully.\n");
            for (struct Sub *current = result_list; current != NULL; current = current->next) {
                printf("File: %s: Line %d, Position %d\n",
                       current->filename,
                       current->line,
                       current->idx);
            }
            break;
        case FILE_OPEN_ERROR:
            printf("Error: Unable to open file.\n");
            break;
        case MEMORY_ALLOCATION_ERROR:
            printf("Error: Memory allocation failed.\n");
            break;
        case INVALID_ARGUMENTS:
            printf("Error: Invalid arguments provided.\n");
            break;
        default:
            printf("Unknown error");
            break;
    }

    free_result_list(result_list);
    return 0;
}
