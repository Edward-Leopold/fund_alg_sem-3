#include <stdio.h>
#include <math.h>

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
        if (ch >= '0' || ch <= '9' || ch == '.'){
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

int getOpts(int argc, char** argv, double* epsilon){
    if (argc != 2){
        if (argc < 2){
            return 101; // not enough arguements
        } else{
            return 102; // to much arguements
        }
    }

    char* eps_option = argv[1];
    if (parse_double(eps_option, epsilon)) {
        return 301;
    }
    if (*epsilon == 0){
        return 302;
    }

    return 0;
}

int main(int argc, char** argv){
    double eps = 0;

    int err_status = getOpts(argc, argv, &eps);
    if(err_status){ // handling errors from cli input
        switch (err_status)
        {
        case 101:
            printf("%s \n", "Not enough number of argumnets. You have to pass two arguements.");
            break;
        case 102:
            printf("%s \n", "Too many argumnets. You have to pass two arguements.");
            break;
        case 301:
            printf("%s \n", "Invalid value of first arguement. The first arguement must be a valid float number.");
            break;
        case 302:
            printf("%s \n", "Invalid value of first arguement. The first arguement must not be 0.");
            break;
        }
        return 1;
    }
    
    
    return 0;
}