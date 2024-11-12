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



int main(int argc, char** argv){




    return 0;
}