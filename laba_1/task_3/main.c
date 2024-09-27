#include <stdio.h>
#include <math.h>

typedef enum kOpts{
    OPT_Q,
    OPT_M,
    OPT_T
} kOpts;

int parse_int(char* proceeding_number, int* result_number){
    int temp = 0;
    int is_negative = 0;
    if (proceeding_number[0] == '-'){ // parsing negative int number
        is_negative = 1;
    }
    for (int i = is_negative ? 1 : 0; proceeding_number[i]; i++){
        char ch = proceeding_number[i];
        if (ch >= '0' && ch <= '9'){
            temp *= 10;
            temp += ch - '0';
        } else{
            return 1;
        }
    }

    *result_number = is_negative ? -temp : temp;

    return 0;
}

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


int getOpts(int argc, char** argv, kOpts *option, double *num_arguements){
    if (argc < 4){
        return 101; // not enough arguements
    }

    const char* proceeding_option = argv[1]; // getting the flag string

    // checking the flag type
    if (proceeding_option[0] == '-' || proceeding_option[1] == '/'){
        switch (proceeding_option[1])
        {
        case 'q':
            *option = OPT_Q;
            break;
        case 'm':
            *option = OPT_M;
            break;
        case 't':
            *option = OPT_T;
            break;
        default:
            return 202; // unknown flag type
            break;
        }
        if(proceeding_option[2]){ // if there are more than 1 sign after '-' or '/' (e. g. -qm)
            return 202; // unknown flag type
        }
    } else{
        return 201; // the arguemnet is not a flag
    }


    switch (*option)
    {
    case 0: // -q flag
        if (argc != 6){
            if (argc < 6){
                return 101;
            } else{
                return 102;
            }
        }
        
        for(int i = 2; argv[i]; i++){
            double d;
            if(parse_double(argv[i], &d)){
                return 301; // invalid arguement value (must be double)
            }
            num_arguements[i - 2] = d;
        }
        break;
    case 1:
        if (argc != 4){
            if (argc < 4){
                return 101;
            } else{
                return 102;
            }
        }

        for(int i = 2; argv[i]; i++){
            int n;
            if(parse_int(argv[i], &n)){
                return 302; // invalid arguement value (must be integer)
            }
            num_arguements[i - 2] = (double)n;
        }
        break;
    case 2:
        if (argc != 6){
            if (argc < 6){
                return 101;
            } else{
                return 102;
            }
        }
        
        for(int i = 2; argv[i]; i++){
            double d;
            if(parse_double(argv[i], &d)){
                return 301; // ivnalid arguement value (must be double)
            }
            
            num_arguements[i - 2] = d;
        }
        break;
    }

    return 0;
}

void quad_equation(double a, double b, double c, double* roots_arr){
    if (a == 0 || b == 0 || c == 0){
        if(a == 0 && b == 0 && c == 0) return;
        if(a == 0){
            if (b != 0 && c != 0) { // bx = -c
                roots_arr[0] = (-c) / b;
                return;
            }
            if (b != 0 && c == 0){ // bx = 0
                roots_arr[0] = 0;
                return;
            }
           return; // c = 0
        }
        if (b == 0){
            if (a != 0 && c != 0){ // a*x^2 = -c
                double right_side = (-c) / a;
                if (right_side < 0) return;
                roots_arr[0] = sqrt(right_side);
                roots_arr[1] = -sqrt(right_side);
                return;
            }
            if (a != 0 && c == 0){ // a*x^2 = 0
                roots_arr[0] = 0;
                return;
            }
            return; // c = 0
        }
    }
    
    double d = pow(b, 2) - 4*a*c;
    
    if (d < 0) return;
    if (d == 0) {
        roots_arr[0] = (-b) / (2*a);
        return;
    } else{
        roots_arr[0] = (-b + sqrt(d)) / (2*a);
        roots_arr[1] = (-b - sqrt(d)) / (2*a);
    }
    return; 
}

void print_roots(double* roots){
    if(roots[0] && roots[1]){
        printf("корни x1 = %f, x2 = %f;\n", roots[0], roots[1]);
    } else if(roots[0]){
        printf("корень x = %f;\n", roots[0]);
    } else{
        printf("нет корней \n");
    }
}

void print_solution(double a, double b, double c){
    double roots[2];
    quad_equation(a, b, c, roots);
    printf("(%f)*x^2 + (%f)*x + (%f)\n", a, b, c);
    print_roots(roots);
    printf("\n");
    return;
}

void handlerOptQ(double* vals){
    // double a, b, c = vals[0], vals[1], vals[2];
    double eps = vals[0];
    double a = vals[1];
    double b = vals[2];
    double c = vals[3];
    printf("%f %f %f \n", a, b, c);

    if (a == b && b == c){
        print_solution(a, b, c);
        return;
    }
    if ((a == b || a == c || b == c)){
        if (a == b){
            print_solution(a, a, c);
            print_solution(a, c, a);
            print_solution(c, a, a);
        }
        if (a == c){
            print_solution(a, b, a);
            print_solution(b, a, a);
            print_solution(a, a, b);
        }
        if (b == c){
            print_solution(a, b, b);
            print_solution(b, a, b);
            print_solution(b, b, a);
        }
        return;
    } else{ // if a != b != c
        print_solution(a, b, c);
        print_solution(a, c, b);
        print_solution(b, a, c);
        print_solution(b, c, a);
        print_solution(c, a, b);
        print_solution(c, b, a);
    }
    return;
}

int main(int argc, char** argv){
    kOpts option = 0;
    double values[4];
    void (*handlers[3])(double*) = {
        handlerOptQ
    };
    
    int err_status = getOpts(argc, argv, &option, values);
    if(err_status){ // handling errors from cli input
        switch (err_status)
        {
        case 101:
            printf("%s \n", "Not enough number of argumnets.");
            break;
        case 102:
            printf("%s \n", "Too many argumnets for this flag.");
            break;
        case 201:
            printf("%s \n", "Invalid value of first arguement. The first arguement must be a flag with starting with '-' or '/' (e. g. '-q').");
            break;
        case 202:
            printf("%s \n", "Unknown flag type.");
            break;
        case 301:
            printf("%s \n", "Ivnalid arguement value (must be double).");
            break;
        case 302:
            printf("%s \n", "Ivnalid arguement value (must be integer).");
            break;
        }
    }



    printf("flag index: %d, arguements: \n", option);
    for (int i = 0; values[i]; i++){
        printf("%f ", values[i]);
    }
    printf("\n");
    handlers[option](values);
    return 0;
}