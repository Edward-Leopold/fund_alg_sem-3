#include <stdio.h>
#include <math.h>

typedef enum kOpts{
    OPT_H,
    OPT_P,
    OPT_S,
    OPT_E,
    OPT_A,
    OPT_F
} kOpts;

int GetOpts(int argc, char** argv, kOpts *option, int *number){
    if (argc != 3){
        if (argc < 3){
            return 101;
        } else{
            return 102;
        }
    }

    const char* proceeding_number = argv[1];
    const char* proceeding_option = argv[2];

    for (int i = 0; proceeding_number[i]; i++){
        char ch = proceeding_number[i];
        if (ch >= '0' && ch <= '9'){
            *number *= 10;
            *number += ch - '0';
        } else{
            return 201;
        }
    }

    if (proceeding_option[0] == '-' || proceeding_option[1] == '/'){
        switch (proceeding_option[1])
        {
        case 'h':
            *option = OPT_H;
            break;
        case 'p':
            *option = OPT_P;
            break;
        case 's':
            *option = OPT_S;
            break;
        case 'e':
            *option = OPT_E;
            break;
        case 'a':
            *option = OPT_A;
            break;
        case 'f':
            *option = OPT_F;
            break;
        default:
            return 203;
            break;
        }
    } else{
        return 202;
    }

    return 0;
}

void HandlerOptH(int number) {
    for (int i = 1; i <= 100; ++i) {
        if (!(i % number)) {
            printf("%d\n", i);
        }
    }
    return;
}

void HandlerOptP(int number) {
    int flag = 1;

    for (int i = 2; i <= sqrt(number); ++i) {
        if (!(number % i)) {
            flag = 0;
        }
    } 

    if (flag) {
        printf("%d is simple\n", number);
    } else {
        printf("%d is not simple\n", number);
    }
}

void HandleOptS(int number) {
    
}

int main(int argc, char** argv){
    kOpts option = 0;
    int number = 0;
    void (*handlers[6])(int) = {
        HandlerOptH,
        HandlerOptP
    };
    
    const int err_status = GetOpts(argc, argv, &option, &number);
    if (err_status){
        switch (err_status)
        {
        case 101:
            printf("%s \n", "Not enough number of argumnets. You have pass to two arguements.");
            break;
        case 102:
            printf("%s \n", "Too many argumnets. You have to pass two arguements.");
            break;
        case 201:
            printf("%s \n", "Invalid value of first arguement. The first arguement must be an integer.");
            break;
        case 202:
            printf("%s \n", "Invalid value of second arguement. The second arguement must be a flag starting with '/' or '-' (example of valid second arguemnet '-h').");
            break;
        case 203:
            printf("%s \n", "Invalid value of second arguement. Unknown flag.");
            break;
        default:
            break;
        }
    }

    handlers[option](number);

    return 0;
}

