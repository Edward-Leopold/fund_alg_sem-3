#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "../err.h"

typedef enum errs{
    MALL0C_ERR = 1,
} errs;

int compare (void const* elem1, void const* elem2){
    return *((int const *)elem1) - *((int const *)elem2);
}

int binary_search(int* arr, int value, int size) {
    int closest = 0;
    int l = 0;
    int r = size - 1;
    int mid;

    while (l <= r) {
        mid = (l + r) / 2;
        if (abs(arr[mid] - value) < abs(arr[closest] - value))
            closest = mid;

        if (arr[mid] == value)
            return mid;

        if (arr[mid] < value)
            l = mid + 1;

        else if (arr[mid] > value)
            r = mid - 1;
    }
    return closest;
}


int main(int argc, char** argv){
    srand(time(NULL));
    
    int down_border = 10, up_border = 12;
    int min_val = -1000, max_val = 1000;
    int size1 = down_border + rand() % (up_border - down_border + 1);
    int size2 = down_border + rand() % (up_border - down_border + 1);

    
    int* arrA = (int*)malloc(sizeof(int) * size1);
    if (arrA == NULL){
        free(arrA);
        printf("malloc error\n");
        return MALL0C_ERR;
    }

    int* arrB = (int*)malloc(sizeof(int) * size2);
    if (arrB == NULL){
        free(arrB);
        printf("malloc error\n");
        return MALL0C_ERR;
    }

    for(int i = 0; i < size1; i++) arrA[i] = min_val + rand() % (max_val - min_val + 1);
    for(int i = 0; i < size2; i++) arrB[i] = min_val + rand() % (max_val - min_val + 1);

    int* arrC = (int*)malloc(sizeof(int) * size1);
    if (arrC == NULL){
        free(arrA);
        free(arrB);
        free(arrC);
        printf("malloc error\n");
        return MALL0C_ERR;
    }

    qsort(arrB, size2, sizeof(int), compare);

    for (int i = 0; i < size1; ++i) {
        arrC[i] = arrA[i] + arrB[binary_search(arrB, arrA[i], size2)];
    }

    printf("Array A\n");
    for(int i = 0; i < size1; ++i) printf("%d ", arrA[i]);
    printf("\n\n");
    printf("Array B\n");
    for(int i = 0; i < size2; ++i) printf("%d ", arrB[i]);
    printf("\n\n");
    printf("Array C\n");
    for(int i = 0; i < size1; ++i) printf("%d ", arrC[i]);
    printf("\n\n");
    free(arrA);
    free(arrB);
    free(arrC);
    return 0;
}