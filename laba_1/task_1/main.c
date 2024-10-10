#include <stdio.h>
#include <math.h>
#include "../err.h"

typedef enum kOpts{
    OPT_H,
    OPT_P,
    OPT_S,
    OPT_E,
    OPT_A,
    OPT_F
} kOpts;

errorCodes getArgs(int argc, char** argv, kOpts *option, int *number){
    if (argc != 3){
        if (argc < 3){
            return NOT_ENOUGH_ARGUEMENTS;
        } else{
            return TOO_MANY_ARGUEMENTS;
        }
    }

    int is_set_num = 0;
    int is_set_flag = 0;

    for (int i = 1; argv[i]; i++){
        const char* proceeding_option = argv[i];
        
        if (proceeding_option[0] == '-' || proceeding_option[0] == '/'){
            if (!proceeding_option[1]){
                return INVALID_ARGUEMENT;
            }
            if (proceeding_option[2]){
                return UNKNOWN_FLAG;
            }
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
                return UNKNOWN_FLAG;
                break;
            }
            is_set_flag = 1;
        } else if(proceeding_option[0] >= '0' && proceeding_option[0] <= '9'){
            int temp = 0;
            for (int j = 0; proceeding_option[j]; j++){
                char ch = proceeding_option[j];
                if (ch >= '0' && ch <= '9'){
                    temp *= 10;
                    temp += ch - '0';
                } else{
                    return INVALID_INT;
                }
            }
            if (temp == 0) {
                return INVALID_ARGUEMENT; // the number must be natural
            }
            *number = temp;
            is_set_num = 1;
        } else{
            return INVALID_ARGUEMENT;
        }
    }

    if (!(is_set_num && is_set_flag)){
        return INVALID_ARGUEMENT; // if (9 9) or (-h -h) were passed
    }

    return NORMAL;
}

int getLenghtOfInt(int n){
    int len = 0;
    do{
        len++;
        n /= 10;
    } while (n != 0);
    
    return len;
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

    return;
}

void HandleOptS(int number) {
    int num = number;
    int i = 1, j, temp; 
    char hex_num[100]; 
    
    while (num != 0) { 
        temp = num % 16; 
         
        if (temp < 10) 
            temp = temp + 48; 
        else
            temp = temp + 55; 
        hex_num[i++] = temp; 
        num = num / 16; 
    } 
    printf("Hexadecimal value is: "); 
    for (j = i - 1; j > 0; j--) 
        printf("%c ", hex_num[j]);
    printf("\n");

    return;
}

void HandleOptE(int number) {
    const int symbols_in_row = 20;
    int max_power = number;
    for (int power = 1; power <= max_power; power++){
        for (int base = 1; base <= 5; base++){
            int value = pow(base, power);

            int value_symbs = getLenghtOfInt(value);
            int base_symbs = (base == 10) ? 2 : 1;
            int power_symbs = (power == 10) ? 2 : 1;
            int blank_symbs = symbols_in_row - value_symbs - base_symbs - power_symbs - 4;

            printf("%d^%d = %d", base, power, value);
            while(blank_symbs != 0){
                printf(" ");
                blank_symbs--;
            }
            printf("| ");
        }
        printf("\n");
    }
    for (int j = symbols_in_row * 5 + 5; j != 0; j--) printf("=");
    printf("\n");
    for (int power = 1; power <= max_power; power++){
        for (int base = 6; base <= 10; base++){
            int value = pow(base, power);

            int value_symbs = getLenghtOfInt(value);
            int base_symbs = (base == 10) ? 2 : 1;
            int power_symbs = (power == 10) ? 2 : 1;
            int blank_symbs = symbols_in_row - value_symbs - base_symbs - power_symbs - 4;

            printf("%d^%d = %d", base, power, value);
            while(blank_symbs != 0){
                printf(" ");
                blank_symbs--;
            }
            printf("| ");
        }
        printf("\n");
    }

    return;
}

void HandleOptA(int number) {
    int sum = 0;
    for (int i = 1; i <= number; i++){
        sum += i;
    }
    printf("%d \n", sum);
    return;
}

void HandleOptF(int number){
    long long factotial = 1;
    for (int i = 1; i <= number; i++){
        factotial *= i;
    }
    printf("%lld \n", factotial);
    return;
}

int main(int argc, char** argv){
    kOpts option = 0;
    int number = 0;
    void (*handlers[6])(int) = {
        HandlerOptH,
        HandlerOptP,
        HandleOptS,
        HandleOptE,
        HandleOptA,
        HandleOptF
    };
    
    const errorCodes parse_err_status = getArgs(argc, argv, &option, &number);
    if (parse_err_status != NORMAL){
        switch (parse_err_status)
        {
        case NOT_ENOUGH_ARGUEMENTS:
            printf("%s \n", "Not enough number of argumnets. You have to pass two arguements.");
            break;
        case TOO_MANY_ARGUEMENTS:
            printf("%s \n", "Too many argumnets. You have to pass two arguements.");
            break;
        case INVALID_ARGUEMENT:
            printf("%s \n", "Invalid arguement have been passed. You have to pass flag and natural number.");
            break;
        case INVALID_INT:
            printf("%s \n", "An error occured while reading integer number.");
            break;
        case UNKNOWN_FLAG:
            printf("%s \n", "Error. Unknown flag.");
            break;
        }
        return 1;
    }

    if(option == OPT_E && number > 10) {
        printf("%s \n", "Invalid number for flag -e. The value of number passed to -e flag must be less than or equal to 10.");
        return 1;
    }
    handlers[option](number);

    return 0;
}

