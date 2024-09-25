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

double factorial(double n){
    double temp = 1;
    for(int i = 1; i <= n; i++) temp *= i;
    return temp;
}

// limits

double e_lim(double n){
    return pow(1 + (1/n), n);
}

double pi_lim(double n){
    return (pow(pow(2, n) * factorial(n), 4.0) / (n * pow(factorial(2*n), 2.0)));
}

double limit(double (*f)(double), double eps){
    double prev_val = f(1);
    double val = f(2);
    double n = 2;
    while (fabs(val - prev_val) > eps){
        prev_val = val;
        n++;
        val = f(n);
    }
    return f(n -= 2);
}

// sequences

double e_seq(double n){
    return (1 / factorial(n));
}

double pi_seq(double n){
    return ((4 * pow(-1, n - 1) / (2*n - 1)));
}

double sum(double (*f)(double), double start_n, double eps){
    double prev_val = f(start_n);
    start_n++;
    double val = prev_val + f(start_n);
    double n = start_n;
    while (fabs(val - prev_val) > eps){
        prev_val = val;
        n++;
        val = prev_val + f(n);
    }
    return val;
}

double product(double (*f)(double), double start_k, double eps){
    double prev_val = f(start_k);
    // double val = f(start_n++);
    double val = prev_val * f(start_k++);
    double k = start_k;
    while (fabs(val - prev_val) > eps){
        prev_val = val;
        k++;
        val = prev_val * f(k);
    }
    return val;
}

// equations

double e_equ(double x){
    return log(x) - 1;
}

double pi_equ(double x){
    return tan(x);
}

double dichotomy(double (*f)(double), double left, double right, double eps){
    double center;
    while(fabs(f(left) - f(right)) > eps){
        center = (right + left) / 2.0;
        if(f(left) * f(center) > 0){
            left = center;
        } else{
            right = center;
        }
    }
    return center;
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
    printf("the result of e limit calculation is %20.20lf \n", limit(e_lim, eps));
    printf("the result of e sequence calculation is %20.20lf \n", sum(e_seq, 0, eps));
    printf("the result of e equation calculation is %20.20lf \n", dichotomy(e_equ, 2.0, 3.0, eps));
    printf("===============================\n");
    printf("the result of pi limit calculation is %20.20lf \n", limit(pi_lim, eps));
    printf("the result of pi sequence calculation is %20.20lf \n", sum(pi_seq, 1, eps));
    printf("the result of e equation calculation is %20.20lf \n", dichotomy(pi_equ, 3.0, 4.0, eps));
    return 0;
}