#define _XOPEN_SOURCE_EXTENDED 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../err.h"

typedef enum kOpts{
    OPT_D,
    OPT_I,
    OPT_S,
    OPT_A
} kOpts;

errorCodes parse_flag(char* proceeding_string, int* flag_array){
    if (proceeding_string[0] == '-' || proceeding_string[0] == '/'){
        int letter_pos = proceeding_string[1] == 'n' ? 2 : 1;
        flag_array[1] = proceeding_string[1] == 'n' ? 1 : 0;
        switch (proceeding_string[letter_pos])
        {
        case 'd':
            flag_array[0] = OPT_D;
            break;
        case 'i':
            flag_array[0] = OPT_I;
            break;
        case 's':
            flag_array[0] = OPT_S;
            break;
        case 'a':
            flag_array[0] = OPT_A;
            break;
        default:
            return UNKNOWN_FLAG;
            break;
        }
        if (proceeding_string[letter_pos + 1]){
            return UNKNOWN_FLAG;
        }
    } else{
        return NOT_A_FLAG;
    }

    return NORMAL;
}

int strLen(char* s){
    int len = 0;
    for (int i = 0; s[i]; i++){
        len++;
    }
    return len;
}

errorCodes getArgs(int argc, char** argv, kOpts* flag, FILE** in_file, FILE** out_file){
    if (argc < 3){
        return NOT_ENOUGH_ARGUEMENTS;
    } else if(argc > 4) {
        return TOO_MANY_ARGUEMENTS;
    }
    
    int flag_array[2]; // [key_option, is_n]
    errorCodes flag_err = parse_flag(argv[1], flag_array); //parsing flag
    if (flag_err != NORMAL){
        return flag_err;
    }
    *flag = flag_array[0];
    int is_n = flag_array[1];

    *in_file = fopen(argv[2], "r");
    if (*in_file == NULL){
        return UNABLE_TO_OPEN_FILE;
    }

    if(is_n){
        if (argc < 4){
            return NOT_ENOUGH_ARGUEMENTS;
        } else if(argc > 4) {
            return TOO_MANY_ARGUEMENTS;
        }

        *out_file = fopen(argv[3], "w");

        if (*out_file == NULL){
            return UNABLE_TO_OPEN_FILE;
        }

        // if the same file
        if (strcmp(realpath(argv[2], NULL), realpath(argv[3], NULL)) == 0){
            return UNABLE_TO_OPEN_FILE;
        }
        
    } else{
        if(argc > 3){
            return TOO_MANY_ARGUEMENTS;
        }
        // creating prefixed file name 
        char* in_file_name = argv[2];
        char out_file_name[5 + strLen(in_file_name)];
        char* prefix = "out_";
        for (int i = 0; prefix[i]; i++){
            out_file_name[i] = prefix[i];
        }
        for (int j = 0; in_file_name[j]; j++){
            out_file_name[j + 4] = in_file_name[j];
        }
        out_file_name[4 + strLen(in_file_name)] = '\0';
        *out_file = fopen(out_file_name, "w");

        if (*out_file == NULL){
            return UNABLE_TO_OPEN_FILE;
        }
    }

    return NORMAL;
}

int handleOptD(FILE *in_file, FILE *out_file){
    int ch = 0;
    while ((ch = fgetc(in_file)) != EOF){
        if(!(ch <= '9' && ch >= '0')){
            if(fputc(ch, out_file) == EOF) {
                return 1;
            }
        }
    }
    
    return 0;
}

int handleOptI(FILE *in_file, FILE *out_file){
    int count = 0;
    int error_int; 
    int ch = 0;
    while((ch = fgetc(in_file))){
        if (ch == '\n' || ch == EOF){
            if (ch == EOF){
                error_int = fprintf(out_file, "%d", count);
                break;
            }
            error_int = fprintf(out_file, "%d\n", count);
            count = 0;
        } else if((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')){
            count++;
        }
        if(error_int < 0) { // fprintf returns negative int if error occurs in writing in file 
            return 1;
        }
    }

    return 0;
}

int handleOptS(FILE *in_file, FILE *out_file){
    int count = 0;
    int error_int; 
    int ch = 0;
    while((ch = fgetc(in_file))){
        if (ch == '\n' || ch == EOF){
            if (ch == EOF){
                error_int = fprintf(out_file, "%d", count);
                break;
            }
            error_int = fprintf(out_file, "%d\n", count);
            count = 0;
        } else if(!(ch >= 'A' && ch <= 'Z') && !(ch >= 'a' && ch <= 'z') && !(ch >= '0' && ch <= '9') && !(ch == ' ')){
            count++;
        }
        if(error_int < 0) { // fprintf returns negative int if error occurs in writing in file 
            return 1;
        }
    }

    return 0;
}

int handleOptA(FILE *in_file, FILE *out_file){
    int error_int; 
    int ch = 0;
    while((ch = fgetc(in_file)) != EOF){
        if((ch >= '0' && ch <= '9') || ch == '\n') { 
            if(fputc(ch, out_file) == EOF){
                return 1;
            }
        } else{ //отличные от символов цифр
            int num = ch;
            int i = 1, j, temp; 
            char hex_num[100]; 
            
            while (num != 0) { 
                temp = num % 16; 
                
                if (temp < 10) 
                    temp = temp + 48; 
                else
                    temp = temp + 55; 
                hex_num[i++] = temp; 
                num = num / 16; 
            } 
            for (j = i - 1; j > 0; j--){
                error_int = fprintf(out_file, "%c", hex_num[j]);
            }
        }
        if(error_int < 0) { // fprintf returns negative int if error occurs in writing in file 
            return 1;
        }
    }

    return 0;
}

int main(int argc, char** argv){
    kOpts flag;
    FILE *in_file;
    FILE *out_file;

    int (*handlers[4])(FILE*, FILE*) = {
        handleOptD,
        handleOptI,
        handleOptS,
        handleOptA
    };

    errorCodes err_status = getArgs(argc, argv, &flag, &in_file, &out_file);
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

    printf("opt_key: %d \n", flag);
    if(handlers[flag](in_file, out_file)) {
        printf("Error writing to file occured\n");
    } else{
        printf("Output file has been successfully changed\n");
    }
    fclose(in_file);
    fclose(out_file);
    return 0;
}