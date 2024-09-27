#include <stdio.h>
#include <math.h>

typedef enum kOpts{
    OPT_Q,
    OPT_M,
    OPT_T
} kOpts;


int parse_int(char* proceeding_number, int* result_number){
    int temp = 0;
    for (int i = 0; proceeding_number[i]; i++){
        char ch = proceeding_number[i];
        if (ch >= '0' && ch <= '9'){
            temp *= 10;
            temp += ch - '0';
        } else{
            return 1;
        }
    }

    *result_number = temp;

    return 0;
}

int parse_double(char* proceeding_number, double* result_number){
    int flag = 0;
    int digits_after_dot = 0;
    int int_num = 0;
    for (int i = 0; proceeding_number[i]; i++){
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

    *result_number = int_num / pow(10, digits_after_dot);

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
    default:
        break;
    }

    return 0;
}

int main(int argc, char** argv){
    kOpts option = 0;
    double values[4];
    int (*handlers[3])(double* []) = {
        
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
    return 0;
}