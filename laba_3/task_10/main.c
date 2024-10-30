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
    MALLOC_ERR,
    REALL0C_ERR,
    INCORRECT_STRING,

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
    *out_file = argv[2];

    return SUCCESS;
}

errCodes clean_string(const char* str, char** result) {
    const int len_original = strlen(str);
    char *cleaned = (char*)malloc(sizeof(char) * (len_original + 1));
    if (!cleaned) return MALLOC_ERR;

    int open_brackets = 0;
    int need_comma = 0;
    int cnt = 0;
    char prev_char = 0;

    for (int i = 0; str[i] != '\n' && str[i] != '\0'; i++) {
        char current = str[i];

        if (isspace(current)) continue;

        if (current == '(') {
            if(!isalpha(prev_char)){
                free(cleaned);
                return INCORRECT_STRING;
            }
            open_brackets++;
            need_comma = 0;
            cleaned[cnt++] = current;
        } else if (current == ')') {
            open_brackets--;
            need_comma = 1;
            cleaned[cnt++] = current;
        } else if (current == ',') {
            if (!need_comma) {
                free(cleaned);
                return INCORRECT_STRING;
            }
            need_comma = 0;
            cleaned[cnt++] = current;
        } else if (isalpha(current)) {
            if (need_comma) { 
                free(cleaned);
                return INCORRECT_STRING;
            }
            if (isalpha(prev_char)) { 
                free(cleaned);
                return INCORRECT_STRING;
            }
            cleaned[cnt++] = current;
            need_comma = 1;
        } else {
            free(cleaned);
            return INCORRECT_STRING;
        }

        prev_char = current;
    }

    if (open_brackets != 0) {
        free(cleaned);
        return INCORRECT_STRING;
    }

    cleaned[cnt] = '\0';
    *result = cleaned;

    return SUCCESS;
}

errCodes read_strings(const char* in_file, char*** result){
    FILE* input = fopen(in_file, "r");
    if (!input) return UNABLE_TO_OPEN_FILE;
    
    int str_count = 0;
    int mem_size = 10;
    char **strings = malloc(sizeof(char*) * (mem_size + 1));
    if (!strings){
        fclose(input);
        return MALLOC_ERR;
    }

    const int len_str = 501;
    char str[len_str];
    while ((fgets(str, len_str, input)) != NULL){
        int is_empty = 1;
        for (int i = 0; str[i]; i++) {
            if (!isspace(str[i])) {
                is_empty = 0;
                break;
            }
        }
        if (is_empty) continue;

        if(str_count == mem_size){
            mem_size *= 2;
            char **temp = realloc(strings, sizeof(char*) * (mem_size + 1));
            if(!temp){
                for (int i = 0; i < str_count; i++) free(strings[i]);
                fclose(input);
                free(strings);
                return REALL0C_ERR;
            }
            strings = temp;
        }

        char *string;
        errCodes clean_status = clean_string(str, &string);
        if (clean_status != SUCCESS){
            for (int i = 0; i < str_count; i++) free(strings[i]);
            fclose(input);
            free(strings);
            return clean_status;
        }

        strings[str_count] = string;
        str_count++;
    }
    
    strings[str_count] = NULL;


    *result = strings;
    fclose(input);
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

    char **strings;
    errCodes read_status = read_strings(in_file, &strings);
    if(read_status != SUCCESS){ // handling errors from cli input
        switch (read_status){
        case MALLOC_ERR:
            printf("Malloc error!.\n");
            break;
        case REALL0C_ERR:
            printf("Realloc error!.\n");
            break;
        case INCORRECT_STRING:
            printf("Incorrect char in sting in input file!.\n");
            break;
        default:
            break;
        }
        return 1;
    }


    for (int i = 0; strings[i]; i++) {
        printf("Processed string %d: %s %ld\n", i + 1, strings[i], strlen(strings[i]));
        free(strings[i]); // освобождение памяти для каждой строки
    }
    free(strings);
    return 0;
}

