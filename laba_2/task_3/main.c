#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <linux/limits.h>


typedef enum errCodes{
    UNABLE_OPEN_FILE,
    MALLOC_ERR,
    REALLOC_ERR,
    SUBSTR_SIZE_ERR,

    SUCCESS
}errCodes;


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
    if (!is_not_same) res = UNABLE_OPEN_FILE;

    return res;
}

int get_len(const char* str){
    int len = 0;
    for (int i = 0; str[i]; i++) len++;
    return len;
}

int compare_strings(const char* str1, const char* str2){
    while (*str2 != '\0'){
        if (*str1 != *str2) return 0;
        str1++;
        str2++;
    }
    return 1;
}

errCodes find_str(FILE* file, const char* str, char*** out_strings){
    int mem_size = 10;
    char** out_msg = malloc(sizeof(char*) * (mem_size + 1));
    if (!out_msg) return MALLOC_ERR;
    int count = 0;

    int str_len = 1000;
    char s[str_len];
    if (str_len < get_len(str)) return SUBSTR_SIZE_ERR;

    int line_index = 1;
    int chars_in_line = 0;
    while(fgets(s, str_len, file)){
        for(int i = 0; s[i]; i++) {
            chars_in_line++;
            if (s[i] == '\n') {
                line_index++;
                chars_in_line = 0;
            }
        }
        
        int len_substr = get_len(str);
        int len_str = get_len(s);
        for(int i = 0; i < len_str - len_substr; i++){
            if (compare_strings(&s[i], str)) {
                char msg[200];
                sprintf(msg, "строка: %d, cимвол: %d", line_index, i + 1);
                if(mem_size < count + 1) {
                    mem_size *= 2;
                    out_msg = realloc(out_msg, sizeof(char*) * (mem_size + 1));
                    if(!out_msg) return REALLOC_ERR;
                }
                out_msg[count] = msg;
                count++; 
            } 
        }
    }   
    out_msg[count] = NULL; 
    printf("%d\n", line_index);

    for (int i = 0; out_msg[i]; i++) printf("%s\n", out_msg[i]);

    return SUCCESS;
}

errCodes find_str_in_files(const char* str, char*** result, const int n, ...){
    char **out_msg = malloc(sizeof(char*) * (11));
    if (!out_msg) return MALLOC_ERR;

    va_list list;
    va_start(list, n);
    for (int i = 0; i < n; i++){
        char* file_name = va_arg(list, char*);
        FILE *file = fopen(file_name, "r");
        if (file == NULL) return UNABLE_OPEN_FILE;

        find_str(file, str, &out_msg);

    }

    return SUCCESS;
}

int main(int argc, char** argv){
    char **result;
    errCodes status = find_str_in_files("nulla", &result, 1, "in1.txt");

    return 0;
}