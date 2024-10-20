#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "../err.h"

typedef enum kOpts{
    OPT_L,
    OPT_R,
    OPT_U,
    OPT_N,
    OPT_C,
}kOpts;

errorCodes parse_int(char* proceeding_number, unsigned int* result_number){
    unsigned int temp = 0;
    for (int i = 0; proceeding_number[i]; i++){
        char ch = proceeding_number[i];
        if (ch >= '0' && ch <= '9'){
            temp *= 10;
            temp += ch - '0';
        } else{
            return INVALID_INT;
        }
    }

    *result_number = temp;

    return SUCCESS;
}

errorCodes parse_flag(const char* proceeding_string, kOpts* flag){
    if (proceeding_string[0] == '-' || proceeding_string[0] == '/'){
        switch (proceeding_string[1])
        {
        case 'l':
            *flag = OPT_L;
            break;
        case 'r':
            *flag = OPT_R;
            break;
        case 'u':
            *flag = OPT_U;
            break;
        case 'n':
            *flag = OPT_N;
            break;
        case 'c':
            *flag = OPT_C;
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

    return SUCCESS;
}

errorCodes getArgs(int argc, char **argv, kOpts *flag, unsigned int* num, char*** strings){
    if (argc < 3) return NOT_ENOUGH_ARGUEMENTS;

    errorCodes parse_flag_status = parse_flag(argv[1], flag);
    if (parse_flag_status != SUCCESS) return parse_flag_status;

    switch (*flag){
    case OPT_C:
        if (argc < 4) return NOT_ENOUGH_ARGUEMENTS;
        *strings = malloc(sizeof(char*) * 2 + sizeof(char*)*(argc - 4));
        if (*strings == NULL) return MALLOC_ERR;
        (*strings)[0] = argv[2];

        errorCodes parse_int_status = parse_int(argv[3], num);
        if (parse_int_status != SUCCESS) return parse_int_status;

        if (argc < 5) break;
        for (int i = 1; i < argc - 3; i++){
            (*strings)[i] = argv[i + 3];
        }
        (*strings)[argc - 3] = NULL;
        break;
    default:
        if (argc > 3) return TOO_MANY_ARGUEMENTS;
        *strings = malloc(sizeof(char*) * 2); 
        if (*strings == NULL) return MALLOC_ERR;
        (*strings)[0] = argv[2];
        (*strings)[1] = NULL;
        break;
    } 

    return SUCCESS;
}

int my_len(const char* str){
    int len = 0;
    for (int i = 0; str[i]; i++) len++;
    return len;
}

int handlerFlagL(const char* str, int *len){
    *len = my_len(str);
    return 0;
}

int handlerFlagR(const char* str, char** reversed){
    int len = my_len(str);
    for (int i = 0; i < len; i++) (*reversed)[i] = str[len - i - 1];
    (*reversed)[len] = '\0';
    return 0; 
}

int handlerFlagU(const char* str, char** uppercased){
    for (int i = 0; str[i]; i++) (*uppercased)[i] = (i % 2 == 0) ? str[i] : toupper(str[i]); 
    return 0; 
}

int handlerFlagN(const char* str, char** modified){
    int len = my_len(str);
    int numbers = 0, letters = 0, others = 0; 
    for (int i = 0; i < len; i++){
        char ch = str[i];
        if (ch >= '0' && ch <= '9') (*modified)[numbers++] = ch;
    }
    letters += numbers;
    for (int i = 0; i < len; i++){
        char ch = str[i];
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) (*modified)[letters++] = ch;
    }
    others += letters;
    for (int i = 0; i < len; i++){
        char ch = str[i];
        if (!isalnum(ch)) (*modified)[others++] = ch;
    }
    (*modified)[len] = '\0';
    return 0; 
}

int handlerOptC(char** strings, int num, char **concat){
    srand(num);
    int totallen = 0;
    int stringsCount = 0;
    for (int i = 0; strings[i]; i++) {
        totallen += my_len(strings[i]);
        stringsCount += 1;
    }

    for (int i = stringsCount - 1; i > 0; i--) {
		int j = rand() % (i + 1);
		char* temp = strings[i];
		strings[i] = strings[j];
		strings[j] = temp;
	}

    char *out = malloc((totallen + 1) * sizeof(char));
    if(out == NULL) return MALLOC_ERR;
    out[totallen] = '\0';

    int ptr = 0;
	for (int i = 0; strings[i]; i++) {
		for (int j = 0; strings[i][j]; j++) {
			out[ptr++] = strings[i][j];
		}
	}

    *concat = out;
    return SUCCESS;
}

int main(int argc, char** argv){
    kOpts flag;
    unsigned int num;
    char** strings;

    const errorCodes parse_err_status = getArgs(argc, argv, &flag, &num, &strings); 
    if (parse_err_status != SUCCESS){
        switch (parse_err_status){
        case NOT_ENOUGH_ARGUEMENTS:
            printf("Not enough number of argumnets for this flag.\n");
            break;
        case TOO_MANY_ARGUEMENTS:
            printf("Too many argumnets for this flag.\n");
            break;
        case INVALID_INT:
            printf("An error occured while reading integer number. The fourth arguement must be natural number.\n");
            break;
        case UNKNOWN_FLAG:
            printf("Error. Unknown flag.\n");
            break;
        case NOT_A_FLAG:
            printf("Error. The first arguement must be a valid flag.\n");
            break;
        case MALLOC_ERR:
            printf("An error occured while allocating memory\n");
            break;
        }
        return 1;
    }

    switch (flag){
    case OPT_L:
        int len = 0;
        handlerFlagL((strings)[0], &len);
        printf("Length of string: %d\n", len);
        break;
    case OPT_R:
        char* reversed = (char*)malloc(sizeof(char) * (my_len(strings[0]) + 1));
        handlerFlagR(strings[0], &reversed);
        printf("Reversed string: %s\n", reversed);
        free(reversed);
        break;
    case OPT_U:
        char* uppercased = (char*)malloc(sizeof(char) * (my_len(strings[0]) + 1));
        handlerFlagU(strings[0], &uppercased);
        printf("With uppercased letters string: %s\n", uppercased);
        free(uppercased);
        break;
    case OPT_N:
        char* modified = (char*)malloc(sizeof(char) * (my_len(strings[0]) + 1));
        handlerFlagN(strings[0], &modified);
        printf("Modified string: %s\n", modified);
        free(modified);
        break;
    case OPT_C:
            char *concat;
            errorCodes status = handlerOptC(strings, num, &concat);
            if (status != SUCCESS) {
                printf("Error during concatenation\n");
                return 1;
            }
            printf("Concatenated string: %s\n", concat);
            free(concat);
            break;
        break;
    default:
        break;
    }
    free(strings);
    return 0;
}