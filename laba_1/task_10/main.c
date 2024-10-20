#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

typedef enum err_codes{
    INVALID_BASE = 1,
} err_codes;


int len(const char* str) {
    int i = 0;
    while (str[i] != '\0') {
        ++i;
    }
    return i;
}

char* gorner(int n, int base){
    int length = (int)(log(n)/log(base)) + 1;
    char* result = (char*)malloc(sizeof(char) * (length + 1));
    if(result == NULL) return NULL;
    result[length] = '\0';
    if(n < base){
       if(n >= 10) result[0] = (n - 10) + 'a';
       else result[0] = n + '0';
    }
    while(n >= base){
        int div_remain = n % base;
        if(div_remain < 10){
            result[--length] = '0' + div_remain;
        } else{
            result[--length] = 'a' + (div_remain - 10);
        }
        n /= base;
        if(n < base){
            if(n >= 10) result[0] = (n - 10) + 'a';
            else result[0] = n + '0';
        }
    }
    return result;
}

int conversion_to_10(const char *number, int base, long long* result) {
    long long ans = 0;
    int d;
    int sign = 1;
    for (int i = 0; number[i]; i++) {
        if (number[i] == '-') {
            sign = -1;
            continue;
        }

        if ('0' <= number[i] && number[i] <= '9') {
            d = number[i] - '0';
        } 
         else if ('A' <= number[i] && number[i] <= 'Z') {
            d = number[i] - 'a' + 10; 
        } else {
            return 1;
        }
        if (d >= base) return 1;

        long long t = ans;
        ans = ans * base;
        if (ans / base != t) return 1;
        if (sign == 1 && ans > LONG_MAX - d) return 1;
        if (sign == -1 && ans < LONG_MIN + d) return 1;

        ans += sign * d;
    }
    *result = ans;
    return 0;
}


int main(int argc, char** argv){
    int base;
    scanf("%d", &base);
    if(base < 2 || base > 36){
        printf("Invalid base of numbers\n");
        return INVALID_BASE;
    }

    char str[256];
    int max = -INFINITY;
    char max_str[256];
    while (1) {
        scanf("%s", str);
        if (strcmp(str, "Stop") == 0) {
            break;
        }
        
    }




    return 0;
}