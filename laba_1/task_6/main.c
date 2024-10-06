#include <stdio.h>
#include <math.h>
#include "../err.h"

int parse_double(char* proceeding_number, double* result_number){
    int flag = 0;
    int digits_after_dot = 0;
    int int_num = 0;
    int is_negative = 0;
    if (proceeding_number[0] == '-'){ // parsing negative double number
        is_negative = 1;
    }
    for (int i = is_negative ? 1 : 0; proceeding_number[i]; i++){
        char ch = proceeding_number[i];
        if ((ch >= '0' && ch <= '9') || ch == '.'){
            if (ch == '.' && flag) return 201;
            if (ch == '.'){
                flag = 1;
                continue;
            }
            if (flag){
                digits_after_dot++;
            }
            int_num *= 10;
            int_num += ch - '0';
        } else {
            return 1;
        }
    }

    double temp = int_num / pow(10, digits_after_dot);
    *result_number = is_negative ? -temp : temp;

    return 0;
}

errorCodes getOpts(int argc, char** argv, double* epsilon){
    if (argc != 2){
        if (argc < 2){
            return NOT_ENOUGH_ARGUEMENTS; // not enough arguements
        } else{
            return TOO_MANY_ARGUEMENTS; // to much arguements
        }
    }

    char* eps_option = argv[1];
    if (parse_double(eps_option, epsilon)) {
        return INVALID_DOUBLE;
    }
    if (*epsilon <= 0){
        return INVALID_EPSILON;
    }

    return NORMAL;
}

int main(int argc, char** argv){
    double eps = 0;

    errorCodes err_status = getOpts(argc, argv, &eps);
    if(err_status != NORMAL){ // handling errors from cli input
        switch (err_status)
        {
        case NOT_ENOUGH_ARGUEMENTS:
            printf("%s \n", "Not enough number of argumnets. You have to pass one arguement.");
            break;
        case TOO_MANY_ARGUEMENTS:
            printf("%s \n", "Too many argumnets. You have to pass one arguement.");
            break;
        case INVALID_DOUBLE:
            printf("%s \n", "Invalid value of first arguement. The first arguement must be a valid float number.");
            break;
        case INVALID_EPSILON:
            printf("%s \n", "Invalid value of first arguement. The first arguement must be greater than 0.");
            break;
        }
        return 1;
    }
    
    
    return 0;
}