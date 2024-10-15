#define _XOPEN_SOURCE_EXTENDED 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "../err.h"

typedef enum kOpts{
    OPT_R,
    OPT_A
} kOpts;

typedef enum errHandlersCodes{
    MALLOC_ERR = 1
} errHandlersCodes;

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
    errorCodes res = NORMAL;
    char *a_path_pointer = realpath(a, NULL);
    char *b_path_pointer = realpath(b, NULL); 
    if (!a_path_pointer || !b_path_pointer){
        res = UNABLE_TO_OPEN_FILE;
    } else if(strcmp(a_path_pointer, b_path_pointer) == 0){
        res = UNABLE_TO_OPEN_FILE;
    }

    if (a_path_pointer) free(a_path_pointer);
    if (b_path_pointer) free(b_path_pointer);
    return res;
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

void next_lexem(int* c, FILE* f){
    while((*c = fgetc(f)) != EOF){
        if(!(*c == ' ' || *c == '\t' || *c == '\n')) break;
    }
    fseek(f, -1, SEEK_CUR);
}

int handlerOptR(FILE *files[3]){
    FILE *in_file1 = files[0];
    FILE *in_file2 = files[1]; 
    FILE *out_file = files[2];
    int ch1 = 1, ch2 = 1;
    int index_lexem = 1;
    
    next_lexem(&ch1, in_file1);
    next_lexem(&ch2, in_file2);

    while (ch1 != EOF && ch2 != EOF){
        if (index_lexem % 2 == 1){
            while((ch1 = fgetc(in_file1)) != EOF){
                if((ch1 == ' ' || ch1 == '\n' || ch1 == 't')) break;
                if(fputc(ch1, out_file) == EOF) return 1;    
            }
            if(fputc(' ', out_file) == EOF) return 1;
            next_lexem(&ch1, in_file1);
            index_lexem++;
        } else{
            while((ch2 = fgetc(in_file2)) != EOF){
                if((ch2 == ' ' || ch2 == '\n' || ch2 == 't')) break;
                if(fputc(ch2, out_file) == EOF) return 1;    
            }
            if(fputc(' ', out_file) == EOF) return 1;
            next_lexem(&ch2, in_file2);
            index_lexem++;
        }
    }
    
    int ch = (ch1 != EOF) ? ch1 : ch2;
    FILE *in_file = (ch1 != EOF) ? in_file1 : in_file2;
    
    while((ch = fgetc(in_file)) != EOF){
        if (ch == ' ' || ch == '\n' || ch == '\t') {
            next_lexem(&ch, in_file);
            if(ch == EOF) break;
            if(fputc(' ', out_file) == EOF) return 1; 
            continue;    
        }
        if(fputc(ch, out_file) == EOF) return 1;
    }
    
    return 0;
}



char* gorner(int n, int base){
    int length = (int)(log(n)/log(base)) + 1;
    char* result = (char*)malloc(sizeof(char) * (length + 1));
    if(result == NULL) return NULL;
    result[length] = '\0';
    if(n < base){
       if(n >= 10) result[0] = (n - 10) + 'a';
       else result[0] = n + '0';
    }
    while(n >= base){
        int div_remain = n % base;
        if(div_remain < 10){
            result[--length] = '0' + div_remain;
        } else{
            result[--length] = 'a' + (div_remain - 10);
        }
        n /= base;
        if(n < base){
            if(n >= 10) result[0] = (n - 10) + 'a';
            else result[0] = n + '0';
        }
    }
    return result;
}

int handlerOptA(FILE *files[3]){
    FILE *in_file = files[0];
    FILE *output = files[1];
    int c;
    int index_lexem = 1;
    next_lexem(&c, in_file);
    while (1){
        c = fgetc(in_file);
        if(c == EOF) break;
        if(index_lexem % 10 == 0){
            while (c != EOF && c != ' ' && c != '\n' && c != '\t') {
                char* converted = gorner(c, 4);
                if (converted == NULL) return MALLOC_ERR;
                fprintf(output, "%s", converted);
                free(converted);
                c = fgetc(in_file);
            }
            next_lexem(&c, in_file);
            index_lexem++;  
        }
        else if(index_lexem % 2 == 0 && index_lexem % 10 != 0){
            while (c != EOF && c != ' ' && c != '\n' && c != '\t') {
                char tmp = tolower(c);
                fputc(tmp, output);
                c = fgetc(in_file);
            }
            next_lexem(&c, in_file);
            index_lexem++;  
        }
        else if(index_lexem % 5 == 0 && index_lexem % 10 != 0){
            while (c != EOF && c != ' ' && c != '\n' && c != '\t') {
                char* converted = gorner(c, 8);
                if (converted == NULL) return MALLOC_ERR;
                fprintf(output, "%s", converted);
                free(converted);
                c = fgetc(in_file);
            }
            next_lexem(&c, in_file);
            index_lexem++;  
        }
        else{
            if(c == ' ' || c=='\t' || c=='\n') next_lexem(&c, in_file);
            else{
                while (c != EOF){
                    if (c == ' ' || c == '\t' || c == '\n') break;
                    fputc(c, output);
                    c = fgetc(in_file);
                }
                if (c == EOF) break;
                next_lexem(&c, in_file);
            }
            index_lexem++;
        }
        if (c == EOF) break;
        fputc(' ', output);
    }
    return 0;
}


int main(int argc, char** argv){
    kOpts flag;
    FILE *files[3] = {NULL, NULL, NULL};
    int (*handlers[2])(FILE *files[3]) = {
        handlerOptR,
        handlerOptA
    };

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

    printf("opt_key: %d \n", flag);
    if(handlers[flag](files)) {
        printf("Error writing to file occured\n");
    } else{
        printf("Output file has been successfully changed\n");
    }

    for (int i = 0; i < 3; i++) {
        if (files[i]) {
            fclose(files[i]);
        }
    }
    return 0;
}

