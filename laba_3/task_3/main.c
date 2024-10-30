#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<math.h>
#include <limits.h>
#include <linux/limits.h>
#include "err.h"

typedef enum kOpts{
    OPT_A,
    OPT_D
} kOpts;

typedef struct Employee{
    int id;
    char *name;
    char *surname;
    double salary;
} Employee;

errorCodes is_same_file(char *a, char *b){
    errorCodes res = SUCCESS;
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

errorCodes parse_flag(char* proceeding_string, kOpts* flag){
    if (proceeding_string[0] == '-' || proceeding_string[0] == '/'){
        switch (proceeding_string[1]){
        case 'a':
            *flag = OPT_A;
            break;
        case 'd':
            *flag = OPT_D;
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

errorCodes getArgs(int argc, char** argv, char** in_file, char **out_file, kOpts* flag){
    if (argc < 4) return NOT_ENOUGH_ARGUEMENTS;
    else if(argc > 4) return TOO_MANY_ARGUEMENTS;

    errorCodes flag_status = parse_flag(argv[2], flag);
    if (flag_status != SUCCESS) return flag_status;

    errorCodes files_status = is_same_file(argv[1], argv[3]);
    if (files_status != SUCCESS) return files_status;

    *in_file = argv[1];
    *out_file = argv[3];

    return SUCCESS;
}

void next_lexem(int* c, FILE* f){
    while((*c = fgetc(f)) != EOF){
        if(!(*c == ' ' || *c == '\t' || *c == '\n')) break;
    }
    fseek(f, -1, SEEK_CUR);
}

errorCodes sort(const char* in_file, const char* out_file, const kOpts flag, Employee*** result){
    FILE* in = fopen(in_file, "r");
    FILE* out = fopen(out_file, "w");
    if(!in || !out){
        if (in) fclose(in);
        if (out) fclose(out);
        return UNABLE_TO_OPEN_FILE;
    }

    int emp_count = 0;
    int mem_size = 10;
    Employee** list = malloc(sizeof(Employee*) * (mem_size + 1));

    const int size_of_line = 10000;
    char str[size_of_line];
    while ((fgets(str, size_of_line, in)) != NULL){
        if(emp_count == mem_size){
            mem_size *= 2;
            Employee **temp = (Employee**)realloc(list, sizeof(Employee*) * (mem_size + 1));
            if(temp == NULL){
                for (int i = 0; i < emp_count; i++) {
                    free(list[i]->name);
                    free(list[i]->surname);
                    free(list[i]);
                }
                free(list);
                fclose(in);
                fclose(out);
                return REALLOC_ERR;
            }
            list = temp;
        }

        Employee* emp = malloc(sizeof(Employee));
        if(!emp){
            for (int i = 0; i < emp_count; i++) {
                free(list[i]->name);
                free(list[i]->surname);
                free(list[i]);
            }
            free(list);
            fclose(in);
            fclose(out);
            return MALLOC_ERR;
        }

        emp->name = malloc(sizeof(char) * (100 + 1));
        emp->surname = malloc(sizeof(char) * (100 + 1));
        if(!emp->name || !emp->surname){
            if (emp->name) free(emp->name);
            if (emp->surname) free(emp->surname);
            for (int i = 0; i < emp_count; i++) {
                free(list[i]->name);
                free(list[i]->surname);
                free(list[i]);
            }
            free(list);
            fclose(in);
            fclose(out);
            return MALLOC_ERR;
        }

        int scan_status = sscanf(str, "%d %100s %100s %lf", &(emp->id), emp->name, emp->surname, &(emp->salary)); // добавить проверку status = 0
        if (scan_status != 4){
            free(emp->name);
            free(emp->surname);
            free(emp);
            continue;
        }

        list[emp_count] = emp;
        emp_count++;
    }
    list[emp_count] = NULL;
    
    *result = list;

    fclose(in);
    fclose(out);
    return SUCCESS;
}

int main(int argc, char** argv){
    kOpts flag;
    char* in_file;
    char* out_file;
    
    errorCodes err_status = getArgs(argc, argv, &in_file, &out_file, &flag);
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
        case NOT_A_FLAG:
            printf("Invalid value of first arguement. The first arguement must be a flag with starting with '-' or '/'.\n");
            break;
        case UNKNOWN_FLAG:
            printf("Unknown flag type.\n");
            break;
        default:
            break;
        }
        return 1;
    }

    Employee** res;
    sort(in_file, out_file, flag, &res);

    for(int i = 0; res[i]; i++) printf("%d %s %s %lf\n", res[i]->id, res[i]->name, res[i]->surname, res[i]->salary);
    
    return 0;
}