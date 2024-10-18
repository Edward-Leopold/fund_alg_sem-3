#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../err.h"

int parse_int(char* proceeding_number, int* result_number){
    int temp = 0;
    int is_negative = 0;
    if (proceeding_number[0] == '-'){ // parsing negative int number
        is_negative = 1;
    }
    for (int i = is_negative ? 1 : 0; proceeding_number[i]; i++){
        char ch = proceeding_number[i];
        if (ch >= '0' && ch <= '9'){
            if (temp > (INT_MAX - (ch - '0')) / 10) {
                return 1; 
            }
            temp *= 10;
            temp += ch - '0';
        } else{
            return 1;
        }
    }

    *result_number = is_negative ? -temp : temp;

    return 0;
}

errorCodes getArgs(int argc, char** argv, int *a, int *b){
    if(argc < 3) return NOT_ENOUGH_ARGUEMENTS;
    else if(argc > 3) return TOO_MANY_ARGUEMENTS;

    if(parse_int(argv[1], a)) return INVALID_INT;
    if(parse_int(argv[2], b)) return INVALID_INT;
    
    if (*a >= *b) return INVALID_ARGUEMENT;

    return NORMAL;
}
 
int main(int argc, char** argv){
    int a, b;
    
    errorCodes err_status = getArgs(argc, argv, &a, &b);
    if(err_status != NORMAL){ // handling errors from cli input
        switch (err_status){
        case NOT_ENOUGH_ARGUEMENTS:
            printf("%s \n", "Not enough number of argumnets.");
            break;
        case TOO_MANY_ARGUEMENTS:
            printf("%s \n", "Too many argumnets.");
            break;
        case INVALID_INT:
            printf("%s \n", "Ivnalid arguement integer value (the number must be integer).");
            break;
        case INVALID_ARGUEMENT:
            printf("%s \n", "Ivnalid arguement value, the second number must be greater than first.");
            break;
        }
        return 1;
    } 

    printf("%d, %d\n", a, b);
    const int n = 100;
    int array[n];
    for(int i = 0; i < n; ++i) {
        array[i] = a + rand()%(b - a);
        printf("%d ", array[i]);
    }
    printf("\n");
    printf("\n");

    int max_pos = 0;
    int min_pos = 0;

    for(int i = 1; i < n; ++i){
        if(array[i] < array[min_pos]) min_pos = i;
        else if(array[i] > array[max_pos]) max_pos = i;
    }

    printf("max: %d, min: %d\n", array[max_pos], array[min_pos]);
    printf("\n");
    
    int temp;
    temp = array[max_pos];
    array[max_pos] = array[min_pos];
    array[min_pos] = temp;

    for(int i = 0; i < n; ++i) printf("%d ", array[i]);
    printf("\n"); 

    return 0;
}