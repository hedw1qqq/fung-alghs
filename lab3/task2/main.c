#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct {
    int size;
    double *coords;
} Vector;

typedef enum {
    ok,
    invalid_input,
    memory_allocation_error
} errors;

typedef double (*norm_pointer)(Vector *, void *);

double norm1(Vector *x, void *params) {
    double max_x = fabs(x->coords[0]);
    for (int i = 0; i < x->size; ++i) {
        if (fabs(x->coords[i]) > max_x) {
            max_x = fabs(x->coords[i]);
        }
    }
    return max_x;
}

double norm2(Vector *x, void *params) {
    int p = *(int *) params;
    double sum = 0.0;
    for (int i = 0; i < x->size; i++) {
        sum += pow(fabs(x->coords[i]), p);
    }
    return pow(sum, 1.0 / p);
}

double norm3(Vector *x, void *params) {
    double **matrix = (double **) params;
    double sum = 0.0;
    for (int i = 0; i < x->size; i++) {
        for (int j = 0; j < x->size; j++) {
            sum += matrix[i][j] * x->coords[j] * x->coords[i];
        }
    }
    return sqrt(sum);
}

errors create_vector(Vector *v, int size, const double *coords) {
    if (size <= 0) return invalid_input;

    v->coords = (double *) malloc(size * sizeof(double));
    if (!v->coords) return memory_allocation_error;

    v->size = size;
    for (int i = 0; i < size; i++) {
        v->coords[i] = coords[i];
    }
    return ok;
}

void free_vector(Vector *v) {
    if (v && v->coords) {
        free(v->coords);
        v->coords = NULL;
        v->size = 0;
    }
}

errors find_max_norm_vectors(Vector **result_vectors, int *result_count,
                             const Vector *vectors, int vector_count,
                             norm_pointer norm, ...) {
    if (!vectors || !result_vectors || vector_count <= 0) {
        return invalid_input;
    }

    va_list args;
    va_start(args, norm);
    void *params = va_arg(args, void*);

    double max_norm = norm((Vector *) &vectors[0], params);
    *result_count = 1;

    for (int i = 1; i < vector_count; i++) {
        double current_norm = norm((Vector *) &vectors[i], params);
        if (current_norm > max_norm) {
            max_norm = current_norm;
            *result_count = 1;
        } else if (fabs(current_norm - max_norm) < 1e-10) {
            (*result_count)++;
        }
    }

    *result_vectors = (Vector *) malloc(*result_count * sizeof(Vector));
    if (!*result_vectors) {
        va_end(args);
        return memory_allocation_error;
    }

    int result_idx = 0;
    for (int i = 0; i < vector_count; i++) {
        double current_norm = norm((Vector *) &vectors[i], params);
        if (fabs(current_norm - max_norm) < 1e-10) {
            errors err = create_vector(&(*result_vectors)[result_idx],
                                       vectors[i].size,
                                       vectors[i].coords);
            if (err != ok) {
                for (int j = 0; j < result_idx; j++) {
                    free_vector(&(*result_vectors)[j]);
                }
                free(*result_vectors);
                va_end(args);
                return err;
            }
            result_idx++;
        }
    }

    va_end(args);
    return ok;
}

void print_vector(const Vector *v) {
    printf("(");
    for (int i = 0; i < v->size; i++) {
        printf("%f", v->coords[i]);
        if (i < v->size - 1) printf(", ");
    }
    printf(")\n");
}

int main() {
    double coords1[] = {1.0, 2.0, 3.0};
    double coords2[] = {4.0, 5.0, 6.0};
    double coords3[] = {6.0, 7.0, 6.0};

    Vector vectors[3];
    if (create_vector(&vectors[0], 3, coords1) != ok ||
        create_vector(&vectors[1], 3, coords2) != ok ||
        create_vector(&vectors[2], 3, coords3) != ok) {
        printf("Error creating vectors\n");
        return 1;
    }

    Vector *result_vectors;
    int result_count;
    errors err = find_max_norm_vectors(&result_vectors, &result_count,
                                       vectors, 3, norm1, NULL);

    if (err == ok) {
        printf("Vectors with maximum norm1:\n");
        for (int i = 0; i < result_count; i++) {
            print_vector(&result_vectors[i]);
        }

        for (int i = 0; i < result_count; i++) {
            free_vector(&result_vectors[i]);
        }
        free(result_vectors);
    }

    int p = 2;
    err = find_max_norm_vectors(&result_vectors, &result_count,
                                vectors, 3, norm2, &p);

    if (err == ok) {
        printf("\nVectors with maximum norm2 (p=2):\n");
        for (int i = 0; i < result_count; i++) {
            print_vector(&result_vectors[i]);
        }

        for (int i = 0; i < result_count; i++) {
            free_vector(&result_vectors[i]);
        }
        free(result_vectors);
    }

    double **matrix = malloc(3 * sizeof(double *));
    for (int i = 0; i < 3; i++) {
        matrix[i] = malloc(3 * sizeof(double));
        for (int j = 0; j < 3; j++) {
            matrix[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }

    err = find_max_norm_vectors(&result_vectors, &result_count,
                                vectors, 3, norm3, matrix);

    if (err == ok) {
        printf("\nVectors with maximum norm3:\n");
        for (int i = 0; i < result_count; i++) {
            print_vector(&result_vectors[i]);
        }

        for (int i = 0; i < result_count; i++) {
            free_vector(&result_vectors[i]);
        }
        free(result_vectors);
    }

    for (int i = 0; i < 3; i++) {
        free_vector(&vectors[i]);
        free(matrix[i]);
    }
    free(matrix);

    return 0;
}