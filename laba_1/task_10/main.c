#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

typedef enum err_codes{
    SUCCESS,
    INVALID_BASE,
    SCANF_ERR,
    CONVERSION_ERR,
    MALLOC_ERR,
} err_codes;


int len(const char* str) {
    int i = 0;
    while (str[i] != '\0') {
        ++i;
    }
    return i;
}

int charToInt(char c){
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    return -1;
}

char intToChar(int n){
    if (n >= 0 && n <= 9) return '0' + n;
    return 'A' + n - 10;
}

void reverseString(char* str, int length) {
    int i = 0, j = length - 1;
    while (i < j) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

char* back_gorner(long long num, int base){
    int capacity = 10;
    int size = 0;
    char* res = (char*)malloc(sizeof(char) * capacity);
    if (!res) return NULL;

    int minus = (num < 0) ? 1 : 0;
    num = llabs(num);

    if (num == 0) { 
        res[size++] = '0';
    }

    while(num != 0){
        if (size == capacity - 2){
            capacity *= 2;
            char* tmp = (char*)realloc(res, capacity);
            if (!tmp){
                free(res);
                return NULL;
            }
            res = tmp;
        }
        res[size++] = intToChar(num % base);
        num /= base;
    }
    if (minus) {
        res[size++] = '-';
    }
    res[size] = '\0';

    reverseString(res, size);
    return res;
}

err_codes conversion_to_10(const char *number, int base, long long* result) {
    *result = 0;
    int sign = (number[0] == '-' && number[1]) ? 1 : 0;
    int digit;
    for (int i = sign; number[i]; ++i){
        digit = charToInt(number[i]);
        if (digit < 0 || digit >= base){
            return CONVERSION_ERR;
        }
        *result = *result * base + digit;
    }
    if(sign) {
        *result *= -1;
    }
    return SUCCESS;
}

char* removeLeadingZeros(char* str) {
    char* p = str;
    if (str[0] == '-' && str[1] == '0') ++p;
    while (*p == '0') {
        ++p;
    }

    if (*p == '\0') {
        str[0] = '0';
        str[1] = '\0';
    } else {
        if(str[0] == '-'){
            memmove(str + 1, p, strlen(p) + 1);
        } else{
            memmove(str, p, strlen(p) + 1);
        }
    }

    return str;
}


int main(int argc, char** argv){
    int base;
    printf("Enter base: ");
    if (scanf("%d", &base) != 1){
        printf("Error reading base of number!\n");
        return SCANF_ERR;
    }
    if(base < 2 || base > 36){
        printf("Invalid base of numbers\n");
        return INVALID_BASE;
    }

    char str[256];
    long long max = 0;
    char max_str[256];
    long long cur;
    int counter = 0;
    while (1) {
        printf("Enter number in base or \"Stop\" to terminate input: ");
        scanf("%255s", str);
        if (strcmp(str, "Stop") == 0) {
            break;
        }
        ++counter;

        err_codes conv_status = conversion_to_10(str, base, &cur);
        if (conv_status != SUCCESS){
            printf("Conversion error occured\n");
            return INVALID_BASE;
        }
        
        if (llabs(cur) > llabs(max)){
            max = cur;
            removeLeadingZeros(str);
            strcpy(max_str, str);
        }
    }

    if (counter == 0){
        printf("No number was entered. Nothing to convert\n");
        return 0;
    }

    printf("Max number by module: %s\n", max_str);

    int bases[4]= {9, 18, 27, 36};
    for (int i = 0; i < 4; ++i){
        char* num_str = back_gorner(max, bases[i]);
        if (num_str == NULL) {
            printf("Memory allocation error!\n");
            return MALLOC_ERR;
        }
        printf("Max number with base %d: %s\n", bases[i], num_str);
        free(num_str);
        num_str = NULL;
    }



    return 0;
}