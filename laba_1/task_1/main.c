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

    for (int i = 1; argv[i]; i++){
        const char* proceeding_option = argv[i];
        
        if (proceeding_option[0] == '-' || proceeding_option[0] == '/'){
            if (proceeding_option[2]){
                return 203;
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
                return 203;
                break;
            }
        } else if(proceeding_option[0] >= '0' && proceeding_option[0] <= '9'){
            int temp = 0;
            for (int j = 0; proceeding_option[j]; j++){
                char ch = proceeding_option[j];
                if (ch >= '0' && ch <= '9'){
                    temp *= 10;
                    temp += ch - '0';
                } else{
                    return 201;
                }
            }
            if (temp == 0) {
                return 301; // the number must be natural
            }
            *number = temp;
        } else{
            return 202;
        }
    }
    
    return 0;
}

int getLenghtOfInt(int n){
    int len = 0;
    do{
        len++;
        n /= 10;
    } while (n != 0);
    
    return len;
}

int HandlerOptH(int number) {
    for (int i = 1; i <= 100; ++i) {
        if (!(i % number)) {
            printf("%d\n", i);
        }
    }

    return 0;
}

int HandlerOptP(int number) {
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

    return 0;
}

int HandleOptS(int number) {
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

    return 0;
}

int HandleOptE(int number) {
    if (number > 10){
        return 301;
    }

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

    return 0;
}

int HandleOptA(int number) {
    int sum = 0;
    for (int i = 1; i <= number; i++){
        sum += i;
    }
    printf("%d \n", sum);
    return 0;
}

int HandleOptF(int number){
    long long factotial = 1;
    for (int i = 1; i <= number; i++){
        factotial *= i;
    }
    printf("%lld \n", factotial);
    return 0;
}

int main(int argc, char** argv){
    kOpts option = 0;
    int number = 0;
    int (*handlers[6])(int) = {
        HandlerOptH,
        HandlerOptP,
        HandleOptS,
        HandleOptE,
        HandleOptA,
        HandleOptF
    };
    
    const int parse_err_status = GetOpts(argc, argv, &option, &number);
    if (parse_err_status){
        switch (parse_err_status)
        {
        case 101:
            printf("%s \n", "Not enough number of argumnets. You have to pass two arguements.");
            break;
        case 102:
            printf("%s \n", "Too many argumnets. You have to pass two arguements.");
            break;
        case 201:
            printf("%s \n", "Ошибка при считывании числа.");
            break;
        case 202:
            printf("%s \n", "Ошибка, должно быть два аргумента: флаг и число.");
            break;
        case 203:
            printf("%s \n", "Ошибка! неизвестный флаг.");
            break;
        }
        return 1;
    }

    const int run_err_status = handlers[option](number);
    if (run_err_status){
        switch (run_err_status)
        {
        case 301:
            printf("%s \n", "Invalid number for flag -e. The value of number passed to -e flag must be less than or equal to 10.");
            break;
        }
        return 1;
    }
    

    return 0;
}

