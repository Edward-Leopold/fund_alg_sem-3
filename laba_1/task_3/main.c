#include <stdio.h>
#include <math.h>
#include "../err.h"

typedef enum kOpts{
    OPT_Q,
    OPT_M,
    OPT_T
} kOpts;

typedef enum errHandlersCodes{
    INFINITY_OF_ROOTS = -1,
    NO_ROOTS = -2,
    NOT_NATURAL = -3,
    NOT_POSITIVE_SIDES = -4,
} errHandlersCodes;

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

errorCodes getArgs(int argc, char** argv, kOpts *option, double *num_arguements){
    if (argc < 4){
        return NOT_ENOUGH_ARGUEMENTS;
    }

    const char* proceeding_option = argv[1];

    // checking the flag type
    if (proceeding_option[0] == '-' || proceeding_option[0] == '/'){
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
            return UNKNOWN_FLAG; 
            break;
        }
        if(proceeding_option[2]){
            return UNKNOWN_FLAG; 
        }
    } else{
        return NOT_A_FLAG;
    }


    switch (*option)
    {
    case OPT_Q: 
        if (argc != 6){
            if (argc < 6){
                return NOT_ENOUGH_ARGUEMENTS;
            } else{
                return TOO_MANY_ARGUEMENTS;
            }
        }
        
        for(int i = 2; argv[i]; i++){
            double d;
            if(parse_double(argv[i], &d)){
                return INVALID_DOUBLE;
            }
            num_arguements[i - 2] = d;
        }
        break;
    case OPT_M:
        if (argc != 4){
            if (argc < 4){
                return NOT_ENOUGH_ARGUEMENTS;
            } else{
                return TOO_MANY_ARGUEMENTS;
            }
        }

        for(int i = 2; argv[i]; i++){
            int n;
            if(parse_int(argv[i], &n)){
                return INVALID_INT;
            }
            num_arguements[i - 2] = (double)n;
        }
        break;
    case OPT_T:
        if (argc != 6){
            if (argc < 6){
                return NOT_ENOUGH_ARGUEMENTS;
            } else{
                return TOO_MANY_ARGUEMENTS;
            }
        }
        
        for(int i = 2; argv[i]; i++){
            double d;
            if(parse_double(argv[i], &d)){
                return INVALID_DOUBLE; 
            }
            
            num_arguements[i - 2] = d;
        }
        break;
    }

    return NORMAL;
}

// -q handler
int quad_equation(double a, double b, double c, double eps, int* num_of_roots, double roots[2]){
    if (a == 0 || b == 0 || c == 0){
        if(a == 0 && b == 0 && c == 0) {
            return INFINITY_OF_ROOTS;
        }
        if(a == 0){
            if (b != 0 && c != 0) { // bx = -c
                roots[0] = (-c) / b;
            }
            if (b != 0 && c == 0){ // bx = 0
                roots[0] = 0;
            } else{ // c = 0
                return NO_ROOTS;
            }
            *num_of_roots = 1;
        }
        if (b == 0){
            if (a != 0 && c != 0){ // a*x^2 = -c
                double right_side = (-c) / a;
                if (right_side > 0){
                    roots[0] = sqrt(right_side);
                    roots[1] = -sqrt(right_side);
                    *num_of_roots = 2;
                } else{ // sqrt from negative number
                    return NO_ROOTS;
                }
            }
            if (a != 0 && c == 0){ // a*x^2 = 0
                roots[0] = 0;
                *num_of_roots = 1;
            } else{ // c = 0
                return NO_ROOTS;
            }
        }
    } else{
        double d = b*b - 4*a*c;
        if (d > 0 && d <= eps){
            d = 0;
        }
        if (d == 0) {
            roots[0] = (-b) / (2*a);
            *num_of_roots = 1;
        } else if(d > 0){
            roots[0] = (-b + sqrt(d)) / (2*a);
            roots[1] = (-b - sqrt(d)) / (2*a);
            *num_of_roots = 2;
        } else{
            return NO_ROOTS;
        }
    }

    return 0;
}

// -m handler
int handlerOptM(double* vals){
    int n = (int)vals[0];
    int m = (int)vals[1];

    if(n <= 0 || m <= 0) return NOT_NATURAL;

    if (n % m == 0){
        return 1;
    } 
    return 0;
}

// -t handler
int hadlerOptT(double* vals){
    double eps = vals[0];
    double a = vals[1];
    double b = vals[2];
    double c = vals[3];

    int is_triangle = 0;
    
    if (a + b > c || a + c > b || b + c > a) {
        double hyp, cath1, cath2;

        if (a > b && a > c) {
            hyp = a;
            cath1 = b;
            cath2 = c;
        } else if(b > a && b > c){
            hyp = b;
            cath1 = a;
            cath2 = c;
        } else{
            hyp = c;
            cath1 = a;
            cath2 = b;
        }

        double pif = sqrt(cath1*cath1 + cath2*cath2);
        if (fabs(hyp - pif) <= eps) {
            is_triangle = 1;
        }
    }
    if (a <= 0 || b <= 0 || c <= 0) return NOT_POSITIVE_SIDES;

    return is_triangle;
}

void getPermutations(double a, double b, double c, double permutations[6][4]){
    for (int i = 0; i < 6; i++){
        for (int j = i + 1; j < 6; j++){
            if(permutations[i][1] == permutations[j][1] && permutations[i][2] == permutations[j][2]) permutations[j][0] = 0;
        }
    }
}

int main(int argc, char** argv){
    kOpts option = 0;
    double args_values[4];
    
    errorCodes err_status = getArgs(argc, argv, &option, args_values);
    if(err_status != NORMAL){ // handling errors from cli input
        switch (err_status)
        {
        case NOT_ENOUGH_ARGUEMENTS:
            printf("%s \n", "Not enough number of argumnets.");
            break;
        case TOO_MANY_ARGUEMENTS:
            printf("%s \n", "Too many argumnets for this flag.");
            break;
        case NOT_A_FLAG:
            printf("%s \n", "Invalid value of first arguement. The first arguement must be a flag with starting with '-' or '/' (e. g. '-q').");
            break;
        case UNKNOWN_FLAG:
            printf("%s \n", "Unknown flag type.");
            break;
        case INVALID_DOUBLE:
            printf("%s \n", "Ivnalid arguement value (must be double).");
            break;
        case INVALID_INT:
            printf("%s \n", "Ivnalid arguement value (must be integer).");
            break;
        }
        return 1;
    } 

    switch (option){
    case OPT_Q:
        double eps = args_values[0];
        double a = args_values[1];
        double b = args_values[2];
        double c = args_values[3];

        double permuts[6][4] = {
            {1, a, b, c},
            {1, a, c, b},
            {1, b, a, c},
            {1, b, c, a},
            {1, c, a, b},
            {1, c, b, a}
        };
        getPermutations(a, b, c, permuts);
        for (int i = 0; i < 6; i++){
            if(permuts[i][0]){
                double a = permuts[i][1];
                double b = permuts[i][2];
                double c = permuts[i][3];
                printf("(%f)*x^2 + (%f)*x + (%f)\n", a, b, c);
                double roots[2];
                int num_of_roots;
                switch (quad_equation(a, b, c, eps, &num_of_roots, roots)){
                case INFINITY_OF_ROOTS:
                    printf("Бесконечность корней\n");
                    break;
                case NO_ROOTS:
                    printf("Нет корней\n");
                    break;
                default:
                    switch (num_of_roots){
                    case 1:
                        printf("корень x = %f;\n", roots[0]);
                        break;
                    case 2:
                        printf("корни x1 = %f, x2 = %f;\n", roots[0], roots[1]);
                        break;
                    }
                    break;
                }
                printf("\n");
            }
        }
        break;
    case OPT_M:
        int is_div = handlerOptM(args_values);
        if (is_div == NOT_NATURAL){
            printf("Passed numbers for flag -m must be natural\n");
        } else{
            if(is_div) {
                printf("Первое число кратно второму \n");
            } else{
                printf("Первое число не кратно второму \n");
            }
        }
        break;
    case OPT_T:
        int is_triangle = hadlerOptT(args_values);
        if (is_triangle == NOT_POSITIVE_SIDES){
            printf("Sides of treangle must be positive values\n");
        } else{
            if(is_triangle) {
                printf("Могут быть сторонами прямоугольного треугольника\n");
            } else{
                printf("Не могут быть сторонами прямоугольного треугольника\n");
            }
        }
        break;
    }
    return 0;
}