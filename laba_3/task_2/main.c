#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>

typedef struct Vector{
    double *coords;
    int n;
} Vector;

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


int main(){
    Vector* v = create_vector(5, 0.4, 5.8, -0.89, 1.0, 4.0);
    for (int i = 0; i < v->n; i++) printf("%lf ", v->coords[i]);
    printf("\n");
    delete_vector(v);

    return 0;
}