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

typedef enum errHandlersCodes{
    NOT_SIMPLE_NOT_COMPOSITE = 1,
    TOO_BIG_FACTORIAL = 2,
    TOO_BIG_NUMBER_FOR_TABLE = 3, 
    NO_DENOMINATOR = 4
} errHandlersCodes;

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
            if (temp <= 0) {
                return INVALID_INT; // the number must be natural
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

int HandlerOptH(int number, long long ans[]) {
    int temp = 0;
    int is_no_denominator = 1;
    for (int i = number; i <= 100; i += number ){
        ans[temp] = i;
        temp++;
        is_no_denominator = 0;
    }
    if (is_no_denominator) return NO_DENOMINATOR;
    ans[temp] = 0;
    return 0;
}

int HandlerOptP(int number, long long ans[]) {
    if(number == 1){
        return NOT_SIMPLE_NOT_COMPOSITE;
    }

    ans[0] = 1;
    for (int i = 2; i <= sqrt(number); ++i) {
        if (!(number % i)) {
            ans[0] = 0;
            break;
        }
    } 

    ans[1] = 0;
    return 0;
}

int HandlerOptS(int number, long long ans[]) {
    int num = number;
    int i = 1, j, temp; 
    char hex_num[100]; 
    
    while (num != 0) { 
        temp = num % 16; 
         
        temp = (temp < 10) ? temp + '0' : temp + 'A' - 10;
        hex_num[i++] = temp; 
        num = num / 16; 
    } 
    
    int pos = 0;
    for (j = i - 1; j > 0; j--) {
        ans[pos] = hex_num[j];
        pos++;
    }
    ans[pos] = 0;
    return 0;
}

int HandlerOptE(int number, long long ans[]) {
    if (number > 10) {
        return TOO_BIG_NUMBER_FOR_TABLE;
    }

    for (int i = 1; i <= 10; i++) {
        long long int base = i;
        for (int j = 1; j <= number; j++) {
            int position = (i - 1) * number + (j - 1); 
            ans[position] = base;
            base *= i;
        }
    }
    ans[10 * number] = 0; 
    return 0;
}

int HandlerOptA(int number, long long ans[]) {
    int sum = 0;
    for (int i = 1; i <= number; i++){
        sum += i;
    }
    ans[0] = sum;
    ans[1] = 0;
    return 0;
}

int HandlerOptF(int number, long long ans[]){
    if(number > 20) return TOO_BIG_FACTORIAL;
    long long factorial = 1;
    for (int i = 1; i <= number; i++){
        factorial *= i;
    }
    ans[0] = factorial;
    ans[1] = 0;
    return 0;
}

int main(int argc, char** argv){
    kOpts option = 0;
    int number = 0;
    long long answer[101];
    int (*handlers[6])(int, long long[]) = {
        HandlerOptH,
        HandlerOptP,
        HandlerOptS,
        HandlerOptE,
        HandlerOptA,
        HandlerOptF
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
            printf("%s \n", "An error occured while reading integer number. Passed number must be valid int natural number.");
            break;
        case UNKNOWN_FLAG:
            printf("%s \n", "Error. Unknown flag.");
            break;
        }
        return 1;
    }

    int hadler_err_status = handlers[option](number, answer);
    switch (hadler_err_status){
    case NOT_SIMPLE_NOT_COMPOSITE:
        printf("1 is not simple nor composite\n");
        return 0;
    case NO_DENOMINATOR:
        printf("There are no denominator of %d\n", number);
        return 0;
    case TOO_BIG_FACTORIAL:
        printf("Passed number for factorial must be less than 21\n");
        return 1;
    case TOO_BIG_NUMBER_FOR_TABLE:
        printf("Invalid number for flag -e. The value of number passed to -e flag must be less than or equal to 10\n");
        return 1;
    default:
        switch (option) {
                case OPT_H:
                    for (int i = 0; answer[i]; i++){
                        printf("%lld\n", answer[i]);
                    }
                    return 0;
                case OPT_P:
                    if (answer[0]) printf("%d is simple\n", number);
                    else printf("%d is not simple\n", number);
                    break;
                case OPT_S:
                    for (int i = 0; answer[i]; i++) printf("%c ", (char) answer[i]);
                    printf("\n");
                    break;
                case OPT_E:
                    for (int i = 0; answer[i]; i++) {
                        printf("%13lld", answer[i]);
                        if ((i + 1) % number == 0) printf("\n\n");
                    }
                    break;
                default:
                    printf("%lld", answer[0]);
                    printf("\n");
                    break;
            }
            return 0;
    }

    return 0;
}

