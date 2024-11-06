#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <linux/limits.h>

typedef enum errCodes{
    MALLOC_ERR,
    REALLOC_ERR,
    SAME_FILE_ERR,
    IVNALID_NUM_ARGUEMENTS,
    UNABLE_TO_OPEN_FILE,
    
    
    SUCCESS
} errCodes;

typedef struct Student{
    unsigned int id;
    char *first_name;
    char *last_name;
    char *group;
    unsigned char *grades;
} Student;

typedef struct {
    Student *students;
    int size;
} StudentArray;

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

errCodes getArgs(const int argc, const char** argv, char** filename_in, char** filename_out){
    if (argc != 3) return IVNALID_NUM_ARGUEMENTS;

    errCodes err_status = is_same_file(argv[1], argv[2]);
    if (err_status != SUCCESS) return err_status;

    filename_in = argv[1];
    filename_out = argv[2]; 

    return SUCCESS;
}



int main(int argc, char** argv) {
    char *filename_in, *filename_out;

    errCodes args_status = getArgs(argc, argv, &filename_in, &filename_out);
    if (args_status != SUCCESS){
        switch (args_status){
        case IVNALID_NUM_ARGUEMENTS:
            printf("Invalid number of arguements, must be 2\n");
            break;
        case SAME_FILE_ERR:
            printf("Same file for input and output\n");
            break;
        default:
            break;
        }
        return 1;
    }
    

    return 0;
}