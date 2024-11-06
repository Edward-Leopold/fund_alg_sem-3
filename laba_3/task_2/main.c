#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <float.h>

typedef enum keyErrs {
    MALLOC_ERR,
    REALLOC_ERR,

    SUCCESS
} keyErrs;

typedef struct Vector{
    double *coords;
    int n;
} Vector;

typedef double (*NormFunc)(const Vector *, const void *); 

Vector* create_vector(int n, ...){
    Vector* vector = malloc(sizeof(Vector));
    if (!vector) return NULL;
    vector->n = n;
    vector->coords = (double* )malloc(sizeof(double) * n);
    if (!vector->coords) {
        free(vector);
        return NULL;
    }

    va_list list;
    va_start(list, n);
    for (int i = 0; i < n; i++){
        double num = va_arg(list, double);
        if(!isfinite(num)){
            free(vector->coords);
            free(vector);
            va_end(list);
            return NULL;
        }
        (vector->coords)[i] = num;
    }
    va_end(list);

    return vector;
}

void delete_vector(Vector *vector){
    free(vector->coords);
    free(vector);
}

double norm_max(const Vector *v, const void *arg) {
    double max = fabs(v->coords[0]);
    for (int i = 1; i < v->n; i++) {
        if (fabs(v->coords[i]) > max) {
            max = fabs(v->coords[i]);
        }
    }
    return max;
}

double norm_sum(const Vector *v, const void *arg) {
    double p = *((double *)arg);
    double sum = 0.0;
    for (int i = 0; i < v->n; i++) {
        sum += pow(fabs(v->coords[i]), p);
    }
    return pow(sum, 1.0 / p);
}

double norm_matrix(const Vector *v, const void *arg) {
    double **A = (double **) arg;
    double sum = 0.0;
    for (int i = 0; i < v->n; i++) {
        double matrix_sum = 0.0;
        for (int j = 0; j < v->n; j++) {
            sum += v->coords[i] * A[i][j] * v->coords[j];
        }
    }
    return sqrt(sum);
}

keyErrs max_normas(Vector**** result, int n, int num_norms, int num_vectors, ...){
    Vector **vectors = (Vector**)malloc(sizeof(Vector*) * num_vectors);
    if(!vectors) return MALLOC_ERR;
    Vector ***output = (Vector***)malloc(sizeof(Vector**) * num_norms);
    if(!output) {
        free(vectors);
        return MALLOC_ERR;
    }

    for (int i = 0; i < num_norms; i++) {
        output[i] = (Vector**)malloc(sizeof(Vector*) * (num_vectors + 1));
        if (!output[i]) {
            for (int j = 0; j < i; j++) free(output[j]);
            free(vectors);
            free(output);
            return MALLOC_ERR;
        }
    }

    va_list list;
    va_start(list, num_vectors);
    for (int i = 0; i < num_vectors; i++) {
        vectors[i] = va_arg(list, Vector*);
    }

    for (int k = 0; k < num_norms; k++) {
        NormFunc norm_func = va_arg(list, NormFunc); 
        void *norm_arg = va_arg(list, void*); 

        double max_value = -INFINITY;
        int max_count = 0;

        for (int i = 0; i < num_vectors; i++) {
             double norm_value = norm_func(vectors[i], norm_arg); 

            if (norm_value > max_value) {
                max_value = norm_value;
                max_count = 0;
                output[k][max_count] = vectors[i];
                max_count++;
            } else if (norm_value == max_value) {
                output[k][max_count] = vectors[i];
                max_count++;
            }
        }

        Vector** temp = (Vector**)realloc(output[k], sizeof(Vector*) * (max_count + 1));
        if (!temp) {
            for (int j = 0; j < num_norms; j++) free(output[j]);
            free(output);
            free(vectors);
            va_end(list);
            return REALLOC_ERR;
        }
        output[k] = temp;

        output[k][max_count] = NULL;
    }
    va_end(list);
    free(vectors);
    *result = output;
    return SUCCESS;
}

int main(){
    Vector* v1 = create_vector(5, 1.0, 1.0, 2.0, 1.0, 3.0);       
    Vector* v2 = create_vector(5, 4.0, 2.0, 2.0, 2.0, 2.0);     
    Vector* v3 = create_vector(5, 1.0, 2.0, 5.0, 1.0, 1.0);   


    int num_norms = 3;
    int num_vectors = 3;
    int n = 5;
    double p = 3.0;
    double **matrix = (double**)malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++) {
        matrix[i] = (double*)malloc(n * sizeof(double));
        for (int j = 0; j < n; j++) {
            matrix[i][j] = (i == j) ? 1.0 : 0.0; 
        }
    }
    Vector ***result;
    keyErrs status = max_normas(&result, n, num_norms, num_vectors, v1, v2, v3, norm_max, NULL, norm_sum, &p, norm_matrix, matrix);
    
    if (status != SUCCESS){
        switch (status){
        case MALLOC_ERR:
            printf("malloc error!\n");
            break;
        case REALLOC_ERR:
            printf("Realloc error!\n");
            break;
        default:
            break;
        }
        delete_vector(v1);
        delete_vector(v2);
        delete_vector(v3);
        for (int i = 0; i < n; i++) free(matrix[i]);
        free(matrix);
        return 1;
    }

    for (int i = 0; i < num_norms; i++){
        printf("Самые длинные векторы для %d переданной нормы:\n", i + 1);
        for (int j = 0; result[i][j]; j++){
            Vector *v = result[i][j];
            for (int k = 0; k < v->n; k++) printf("%lf ", v->coords[k]);
            printf("\n");
        }
        free(result[i]);
    }
    free(result);

    delete_vector(v1);
    delete_vector(v2);
    delete_vector(v3);
    for (int i = 0; i < n; i++) free(matrix[i]);
    free(matrix);
    return 0;
}