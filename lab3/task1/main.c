#include "funcs.h"

int main() {
    int number = -9;

    int base = 2;
    int capacity = 10;
    int count;
    char *result = (char *) malloc(capacity * sizeof(char));

    if (result == NULL) {
        printf("Memory allocation error\n");
        return 1;
    }

    errors status = decimal_into_powern(number, base, &result, &capacity, &count);

    if (status != OK) {
        if (status == INVALID_INPUT) {
            printf("Invalid input: \n");
        } else if (status == INVALID_MEMORY) {
            printf("Memory allocation error\n");
        }
        free(result);
        return 1;
    }

    printf("%d in base %d: %s\n", number, base, result);

    free(result);
    return 0;
}
