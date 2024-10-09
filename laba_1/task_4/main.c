#include <stdio.h>
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

errorCodes getOpts(int argc, char** argv, kOpts* flag, FILE** in_file, FILE** out_file){
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

void handleOptD(FILE *in_file, FILE *out_file){
    char ch = 0;
    while ((ch = (char)fgetc(in_file)) != EOF){
        if(!(ch <= '9' && ch >= '0')){
            fputc(ch, out_file);
            printf("%c", ch);
        }
        
    }
    
    
    return;
}

void handleOptI(FILE *in_file, FILE *out_file){
   
}

void handleOptS(FILE *in_file, FILE *out_file){
   
}

void handleOptA(FILE *in_file, FILE *out_file){
   
}

int main(int argc, char** argv){
    kOpts flag;
    FILE *in_file;
    FILE *out_file;

    void (*handlers[4])(FILE*, FILE*) = {
        handleOptD,
        handleOptI,
        handleOptS,
        handleOptA
    };

    errorCodes err_status = getOpts(argc, argv, &flag, &in_file, &out_file);
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
    handlers[flag](in_file, out_file);
    fclose(in_file);
    fclose(out_file);
    return 0;
}