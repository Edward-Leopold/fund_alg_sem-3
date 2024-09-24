#include <stdio.h>
#include <math.h>

int getOpts(int argc, char** argv, double* epsilon){
    if (argc != 2){
        if (argc < 2){
            return 101; // not enough arguements
        } else{
            return 102; // to much arguements
        }
    }

    const char* number_option = argv[1];
    double int_num = 0;
    int flag = 0;
    int digits_after_dot = 0;

    for (int i = 0; number_option[i]; i++){
        char ch = number_option[i];
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
            return 201;
        }
    }

    *epsilon = int_num / pow(10, digits_after_dot);

    return 0;
}

double e_lim( double eps){
    double e = 2.25;
    double prev_e = 2.0;
    double n = 2;
    while (fabs(e - prev_e) > eps){
        prev_e = e;
        n++;
        e = pow(1 + (1/n), n);
    }
    return e;
}

double e_seq(double eps){
    
}

double e_equ(double eps){
    // уравнения надо решать методом дихотомии
}

int main(int argc, char** argv){
    double eps = 0;
    int err_status = getOpts(argc, argv, &eps);
    if(err_status){ // handling errors from cli input
        switch (err_status)
        {
        case 101:
            printf("%s \n", "Not enough number of argumnets. You have pass to two arguements.");
            break;
        case 102:
            printf("%s \n", "Too many argumnets. You have to pass two arguements.");
            break;
        case 201:
            printf("%s \n", "Invalid value of first arguement. The first arguement must be a valid float number.");
            break;
        }
    }
    // printf("%20.180lf \n", eps);
    printf("the result of calculation is %20.20lf \n", e_lim(eps));


    return 0;
}