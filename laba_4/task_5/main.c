#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>








int main(int argc, char** argv){
    if (argc < 3){
        printf("Not enought arguements. Pass flag and files.\n");
        return 1;
    }
    
    if(strcmp(argv[1], "--calculate") == 0){
        printf("calc!\n");

        for (int i = 2; i < argc; ++i){
            FILE* input = fopen(argv[i], "r");

            

            fclose(input);
        }

    } else if(strcmp(argv[1], "--table") == 0){
        printf("table!\n");
    } else{
        printf("unknown flag!\n");
        return 1;
    }

    return 0;
}