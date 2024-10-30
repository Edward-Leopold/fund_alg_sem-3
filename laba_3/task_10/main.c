#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <linux/limits.h>

typedef enum errCodes {
    NOT_ENOUGH_ARGUEMENTS,
    TOO_MANY_ARGUEMENTS,
    UNABLE_TO_OPEN_FILE,
    SAME_FILE,
    WRITE_TO_FILE_ERR,

    SUCCESS
} errCodes;  

errCodes is_same_file(char *a, char *b){
    errCodes res = SUCCESS;
    char input_path[PATH_MAX];
    char output_path[PATH_MAX];
    realpath(a, input_path);
    realpath(b, output_path);
    int is_not_same = 0;
    
    for (int i = 0; (input_path[i] && output_path[i]); i++) {       
        if (input_path[i] != output_path[i]) is_not_same = 1;
    }   
    if (!is_not_same) res = SAME_FILE;

    return res;
}

errCodes getArgs(int argc, char** argv, char **in_file, char **out_file){
    if (argc < 3) return NOT_ENOUGH_ARGUEMENTS;
    else if(argc > 3) return TOO_MANY_ARGUEMENTS;

    errCodes files_status = is_same_file(argv[1], argv[2]);
    if (files_status != SUCCESS) return files_status;

    *in_file = argv[1];
    *out_file = argv[3];

    return SUCCESS;
}


int main(int argc, char** argv){
    char *in_file;
    char *out_file;

    errCodes err_status = getArgs(argc, argv, &in_file, &out_file);
    if(err_status != SUCCESS){ // handling errors from cli input
        switch (err_status){
        case NOT_ENOUGH_ARGUEMENTS:
            printf("Not enough number of argumnets.\n");
            break;
        case TOO_MANY_ARGUEMENTS:
            printf("Too many argumnets.\n");
            break;
        case SAME_FILE:
            printf("Files are the same \n");
            break;
        default:
            break;
        }
        return 1;
    }

    

    return 0;
}