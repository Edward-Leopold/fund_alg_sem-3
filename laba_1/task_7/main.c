#define _XOPEN_SOURCE_EXTENDED 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../err.h"

typedef enum kOpts{
    OPT_R,
    OPT_A
} kOpts;

errorCodes parse_flag(char* proceeding_string, kOpts* flag){
    if (proceeding_string[0] == '-' || proceeding_string[0] == '/'){
        switch (proceeding_string[1])
        {
        case 'r':
            *flag = OPT_R;
            break;
        case 'a':
            *flag = OPT_A;
            break;
        default:
            return UNKNOWN_FLAG;
            break;
        }
        if (proceeding_string[2]){
            return UNKNOWN_FLAG;
        }
    } else{
        return NOT_A_FLAG;
    }

    return NORMAL;
}

errorCodes is_same_file(char *a, char *b){
    char *a_path_pointer = realpath(a, NULL);
    char *b_path_pointer = realpath(b, NULL); 
    if (!a_path_pointer || !b_path_pointer){
        return UNABLE_TO_OPEN_FILE;
    }
    if (strcmp(a_path_pointer, b_path_pointer) == 0){
        return UNABLE_TO_OPEN_FILE;
    }

    return NORMAL;
}

errorCodes getArgs(int argc, char** argv, kOpts *flag, FILE *files[3]){
    if(argc < 4) return NOT_ENOUGH_ARGUEMENTS;
    else if(argc > 5) return TOO_MANY_ARGUEMENTS;

    char* proceeding_flag = argv[1];
    errorCodes flag_err = parse_flag(proceeding_flag, flag); 
    if(flag_err != NORMAL) return flag_err;

    switch (*flag){
    case OPT_R:
        if(argc < 5) return NOT_ENOUGH_ARGUEMENTS;
        else if(argc > 5) return TOO_MANY_ARGUEMENTS;

        if((is_same_file(argv[2], argv[4]) != NORMAL) || (is_same_file(argv[3], argv[4]) != NORMAL)){
            return UNABLE_TO_OPEN_FILE;
        }

        FILE *in_file_1, *in_file_2, *out_file;
        in_file_1 = fopen(argv[2], "r");
        in_file_2 = fopen(argv[3], "r");
        out_file = fopen(argv[4], "w");
        if (in_file_1 == NULL || in_file_2 == NULL || out_file == NULL){
            return UNABLE_TO_OPEN_FILE;
        }
        files[0] = in_file_1;
        files[1] = in_file_2;
        files[2] = out_file;

        break;
    case OPT_A:
        if(argc < 4) return NOT_ENOUGH_ARGUEMENTS;
        else if(argc > 4) return TOO_MANY_ARGUEMENTS;

        if(is_same_file(argv[2], argv[3]) != NORMAL){
            return UNABLE_TO_OPEN_FILE;
        }

        FILE *in_file, *output_file;
        in_file = fopen(argv[2], "r");
        output_file = fopen(argv[3], "w");
        if (in_file == NULL || output_file == NULL){
            return UNABLE_TO_OPEN_FILE;
        }
        files[0] = in_file;
        files[1] = output_file;

        break;
    }
    return NORMAL;
}


int main(int argc, char** argv){
    kOpts flag;
    FILE *files[3];

    errorCodes err_status = getArgs(argc, argv, &flag, files);
    if(err_status != NORMAL){ // handling errors from cli input
        switch (err_status)
        {
        case NOT_ENOUGH_ARGUEMENTS:
            printf("%s \n", "Not enough number of argumnets.");
            break;
        case TOO_MANY_ARGUEMENTS:
            printf("%s \n", "Too many argumnets.");
            break;
        case UNKNOWN_FLAG:
            printf("%s \n", "Unknown flag");
            break;
        case NOT_A_FLAG:
            printf("%s \n", "Passed arguement is not a flag. The first arguement must be a flag.");
            break;
        case UNABLE_TO_OPEN_FILE:
            printf("Cannot open file \n");
            break;
        case INVALID_ARGUEMENT:
            printf("Invalid arguement has been passed \n");
            break;
        }
        return 1;
    }


    return 0;
}

