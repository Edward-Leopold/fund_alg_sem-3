#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <linux/limits.h>

typedef enum errCodes{
    NOT_ENOUGH_ARGUEMENTS,
    TOO_MANY_ARGUEMENTS,
    MALLOC_ERR,
    REALLOC_ERR,
    SAME_FILE_ERR,
    UNABLE_TO_OPEN_FILE,

    SUCCESS
} errCodes;

errCodes is_same_file(const char *a, const char *b){
    errCodes res = SUCCESS;
    char input_path[PATH_MAX];
    char output_path[PATH_MAX];
    realpath(a, input_path);
    realpath(b, output_path);
    int is_not_same = 0;
    
    for (int i = 0; (input_path[i] && output_path[i]); i++) {       
        if (input_path[i] != output_path[i]) is_not_same = 1;
    }   
    if (!is_not_same) res = SAME_FILE_ERR;

    return res;
}

errCodes getArgs(int argc, char** argv, char** filename){
    if (argc < 2) return NOT_ENOUGH_ARGUEMENTS;
    else if(argc > 2) return TOO_MANY_ARGUEMENTS;

    *filename = argv[1];

    return SUCCESS;
}

int main(int argc, char** argv){
    char* filename = NULL;
    errCodes args_status = getArgs(argc, argv, &filename);
    if (args_status != SUCCESS){
        switch (args_status){
        case NOT_ENOUGH_ARGUEMENTS:
            printf("Not enough number of argumnets.\n");
            break;
        case TOO_MANY_ARGUEMENTS:
            printf("Too many argumnets.\n");
            break;
        default:
            break;
        }
        return 1;
    }


    return 0;
}