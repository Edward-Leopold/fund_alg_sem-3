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

// -q handler
void print_roots(double* roots){
    if(roots[0] && roots[1]){
        printf("корни x1 = %f, x2 = %f;\n", roots[0], roots[1]);
    } else {
        printf("корень x = %f;\n", roots[0]);
    }
}

void quad_equation(double a, double b, double c, double eps){
    double roots[2];
    if (a == 0 || b == 0 || c == 0){
        if(a == 0 && b == 0 && c == 0) {
            printf("нет корней\n");
            return;
        }
        if(a == 0){
            if (b != 0 && c != 0) { // bx = -c
                roots[0] = (-c) / b;
            }
            if (b != 0 && c == 0){ // bx = 0
                roots[0] = 0;
            } else{ // c = 0
                printf("нет корней\n");
                return;
            }
        }
        if (b == 0){
            if (a != 0 && c != 0){ // a*x^2 = -c
                double right_side = (-c) / a;
                if (right_side > 0){
                    roots[0] = sqrt(right_side);
                    roots[1] = -sqrt(right_side);
                } else{ // sqrt from negative number
                    printf("нет корней\n");
                    return;
                }
            }
            if (a != 0 && c == 0){ // a*x^2 = 0
                roots[0] = 0;
            } else{ // c = 0
                printf("нет корней\n");
                return;
            }
        }
    } else{
        double d = pow(b, 2) - 4*a*c;
        printf("d = %f\n", d);
        if (d > 0 && d <= eps){
            d = 0;
        }
        if (d == 0) {
            roots[0] = (-b) / (2*a);
        } else if(d > 0){
            roots[0] = (-b + sqrt(d)) / (2*a);
            roots[1] = (-b - sqrt(d)) / (2*a);
        } else{
            printf("нет корней\n");
            return;
        }
    }
    
    print_roots(roots);

    return;
}

void print_solution(double a, double b, double c, double eps){
    printf("(%f)*x^2 + (%f)*x + (%f)\n", a, b, c);
    quad_equation(a, b, c, eps);
    printf("\n");

    return;
}

void handlerOptQ(double* vals){
    double eps = vals[0];
    double a = vals[1];
    double b = vals[2];
    double c = vals[3];

    if (a == b && b == c){
        print_solution(a, b, c, eps);
        return;
    }
    if ((a == b || a == c || b == c)){
        if (a == b){
            print_solution(a, a, c, eps);
            print_solution(a, c, a, eps);
            print_solution(c, a, a, eps);
        }
        if (a == c){
            print_solution(a, b, a, eps);
            print_solution(b, a, a, eps);
            print_solution(a, a, b, eps);
        }
        if (b == c){
            print_solution(a, b, b, eps);
            print_solution(b, a, b, eps);
            print_solution(b, b, a, eps);
        }
        return;
    } else{ // if a != b != c
        print_solution(a, b, c, eps);
        print_solution(a, c, b, eps);
        print_solution(b, a, c, eps);
        print_solution(b, c, a, eps);
        print_solution(c, a, b, eps);
        print_solution(c, b, a, eps);
    }
    return;
}

// -m handler
void handlerOptM(double* vals){
    int n = (int)vals[0];
    int m = (int)vals[1];

    // printf("%d %d\n", n, m);
    if (n % m == 0){
        printf("Число %d кратно %d \n", n, m);
    } else{
        printf("Число %d не кратно %d \n", n, m);
    }
    return;
}

// -t handler
void hadlerOptT(double* vals){
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

        double pif = sqrt(pow(cath1, 2) + pow(cath2, 2));
        if (fabs(hyp - pif) <= eps) {
            is_triangle = 1;
        }
    }
    if (a <= 0 || b <= 0 || c <= 0) is_triangle = 0;

    if(is_triangle){
        printf("Числа %f, %f, %f могут являться сторонами прямоугольного треугольника \n", a, b, c);
    } else{
        printf("Числа %f, %f, %f не могут являться сторонами прямоугольного треугольника \n", a, b, c);
    }
}

int main(int argc, char** argv){
    kOpts option = 0;
    double values[4];
    void (*handlers[3])(double*) = {
        handlerOptQ,
        handlerOptM,
        hadlerOptT
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
    } else{
        handlers[option](values);
    }
    return 0;
}