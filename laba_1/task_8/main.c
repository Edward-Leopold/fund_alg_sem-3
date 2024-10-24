#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<math.h>
#include <limits.h>
#include <linux/limits.h>
#include "../err.h"

typedef enum errHandlersCodes{
    MALLOC_ERR = 1, 
    REALLOC_ERR = 2,
    WRITE_TO_FILE_ERR = 3,
    INVALID_SYMBOL = 4,
    TOO_BIG_NUM = 5,
    GET_BASE_ERR = -1,
} errHandlersCodes;

errorCodes is_same_file(char *a, char *b){
    errorCodes res = NORMAL;
    char input_path[PATH_MAX];
    char output_path[PATH_MAX];
    realpath(a, input_path);
    realpath(b, output_path);
    int is_not_same = 0;
    
    for (int i = 0; (input_path[i] && output_path[i]); i++) {       
        if (input_path[i] != output_path[i]) is_not_same = 1;
    }   
    if (!is_not_same) res = UNABLE_TO_OPEN_FILE;

    return res;
}


errorCodes getArgs(int argc, char** argv, FILE **in_file, FILE **out_file){
    if(argc < 3) return NOT_ENOUGH_ARGUEMENTS;
    else if(argc > 3) return TOO_MANY_ARGUEMENTS;

    if((is_same_file(argv[1], argv[2]) != NORMAL)){
        return UNABLE_TO_OPEN_FILE;
    }

    *in_file = fopen(argv[1], "r");
    *out_file = fopen(argv[2], "w");
    if (*in_file == NULL || *out_file == NULL){
        return UNABLE_TO_OPEN_FILE;
    }
    
    return NORMAL;
}

int get_base(char* str){
    int is_negative = (str[0] == '-') ? 1 : 0;
    char max_char = str[0 + is_negative];
    for(int i = 0 + is_negative; str[i]; ++i){
        if(str[i] > max_char) max_char = str[i];
    }
    if(isdigit(max_char)){
        return (int)max_char - '0' + 1;
    }
    else if(isalpha(max_char)){
        return (int)max_char - 'a' + 11;
    }
    return GET_BASE_ERR;
}

int validate_lexem(char* str){
    int is_negative = (str[0] == '-' && isalnum(str[1])) ? 1 : 0;

    int start = is_negative;

    while (str[start] == '0' && isalnum(str[start + 1])) {
        start++;
    }

    int i = is_negative;
    while (str[start]) {
        str[i++] = str[start++];
    }
    str[i] = '\0';

    for (int i = is_negative; str[i]; ++i){
        if (!isalnum(str[i])) return 1; 
    }

    return 0;
}

void next_lexem(int* c, FILE* f){
    while((*c = fgetc(f)) != EOF){
        if(!(*c == ' ' || *c == '\t' || *c == '\n')) break;
    }
    fseek(f, -1, SEEK_CUR);
}

int is_space(char c){
    return (c == ' ' || c == '\t' || c == '\n');
}

int conversion_to_10(const char *number, int base, long long* result) {
    long long ans = 0;
    int d;
    int sign = 1;
    for (int i = 0; number[i]; i++) {
        if (number[i] == '-') {
            sign = -1;
            continue;
        }

        if ('0' <= number[i] && number[i] <= '9') {
            d = number[i] - '0';
        } 
         else if ('a' <= number[i] && number[i] <= 'z') {
            d = number[i] - 'a' + 10; 
        } else {
            return 1;
        }
        if (d >= base) return 1;

        long long t = ans;
        ans = ans * base;
        if (ans / base != t) return 1;
        if (sign == 1 && ans > LONG_MAX - d) return 1;
        if (sign == -1 && ans < LONG_MIN + d) return 1;

        ans += sign * d;
    }
    *result = ans;
    return 0;
}


int handler(FILE *in_file, FILE *out_file){
    int c;
    next_lexem(&c, in_file);
    while (1){
        c = fgetc(in_file);
        if(c == EOF) break;
        if(isalnum(c) || c == '-'){
            char* number = (char*)malloc(sizeof(char) * 2);
            if(number == NULL) return MALLOC_ERR;
            int len = 1;
            int memlen = 2;
            number[0] = tolower(c);
            while (!is_space(c = fgetc(in_file)) && c != EOF) {
                if(len + 1 >= memlen){
                    memlen *= 2;
                    number = (char*)realloc(number, memlen);
                    if (number == NULL) {
                        free(number);
                        return REALLOC_ERR;
                    }
                }
                number[len] = tolower(c);
                len++;
            }
            number[len] = '\0';

            if(validate_lexem(number) != 0){
                free(number);
                return INVALID_SYMBOL;
            }
            int base = get_base(number);
            if (base == GET_BASE_ERR) {
                free(number);
                return GET_BASE_ERR;
            }
            long long number_in_10;
            if (conversion_to_10(number, base, &number_in_10) != 0) {
                free(number);
                return TOO_BIG_NUM; 
            }

            if(fprintf(out_file, "%s %d %lld\n", number, base, number_in_10) == EOF){
                free(number);
                return WRITE_TO_FILE_ERR;
            } 
            free(number);

            next_lexem(&c, in_file);
        }else {
            return INVALID_SYMBOL;
        }
        if(c == EOF) break;
    }
    
    return 0;
}

int main(int argc, char* argv[]){
    FILE* in_file = NULL;
    FILE* out_file = NULL;
    errorCodes err_status = getArgs(argc, argv, &in_file, &out_file);
    if(err_status != NORMAL){ // handling errors from cli input
        switch (err_status)
        {
        case NOT_ENOUGH_ARGUEMENTS:
            printf("%s \n", "Not enough number of argumnets.");
            break;
        case TOO_MANY_ARGUEMENTS:
            printf("%s \n", "Too many argumnets.");
            break;
        case UNABLE_TO_OPEN_FILE:
            printf("Cannot open file \n");
            break;
        case INVALID_ARGUEMENT:
            printf("Invalid arguement has been passed \n");
            break;
        default:
            break;
        }
        return 1;
    }

    switch (handler(in_file, out_file)){
    case WRITE_TO_FILE_ERR:
        printf("Error writing to file occured\n");
        break;
    case MALLOC_ERR:
        printf("Error malloc\n");
        break;
    case REALLOC_ERR:
        printf("Error realloc\n");
        break;
    case INVALID_SYMBOL:
        printf("Invalid symbol\n");
        break;
    case TOO_BIG_NUM:
        printf("Too big number in input file\n");
        break;
    case GET_BASE_ERR:
        printf("An error occured while getting base of number\n");
        break;
    default:
        printf("Output file has been successfully changed\n");
        break;
    }

    fclose(in_file);
    fclose(out_file);
    return 0;

}
