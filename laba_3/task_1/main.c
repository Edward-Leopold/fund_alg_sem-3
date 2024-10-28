#include <stdio.h>
#include <stdlib.h>

typedef enum keyErrs {
    MALLOC_ERR,
    BASE_ERR,
    NUM_ERR,
    OVERFLOW_ERR,
    SUCCESS
} keyErrs;

unsigned int decrement(unsigned int num) {
    unsigned int mask = 1;
    while ((num & mask) == 0) {
        num = num ^ mask;
        mask <<= 1;
    }
    return num ^ mask;  
}

unsigned int increment(unsigned int num) {
    unsigned int mask = 1;
    while (num & mask) {
        num = num ^ mask;
        mask <<= 1;
    }
    return num ^ mask; 
}

keyErrs convert_base2r( int num, unsigned int r, char **result){
    if (num < 0) return NUM_ERR;
    else if (num == 0) {
        *result = malloc(sizeof(char) << 1);
        if (*result == NULL) return MALLOC_ERR;
        (*result)[0] = '0';
        (*result)[1] = '\0';
        return SUCCESS;
    }
    if (r > 5 || r < 1) return BASE_ERR;

    unsigned int base = 1 << r;
    unsigned int mask = decrement(base);

    unsigned int size = sizeof(char) << 8;
    char *out = (char*)malloc(size);
    if(out == NULL) return MALLOC_ERR;

    unsigned int index = 0;
    const char chars[] = "0123456789abcdefghijklmnopqrstuv";

    while (num > 0){
        unsigned int current_bits = num & mask;
        out[index] = chars[current_bits];
        index = increment(index);
        num >>= r;
    }

    unsigned int left = 0;
    unsigned int right = decrement(index);
    while(left < right) {
        char temp = out[left];
        out[left] = out[right];
        out[right] = temp;
        left = increment(left);
        right = decrement(right);
    }

    out[index] = '\0';

    *result = out;

    return SUCCESS;
}

int main(){
    char *res; 
    keyErrs status = convert_base2r(87, 3, &res);
    if (status != SUCCESS){
        switch (status){
        case MALLOC_ERR:
            printf("Malloc error occured!\n");
            break;
        case BASE_ERR:
            printf("Base is 2^r and r must be in natural int from 1 to 5\n");
            break;
        case NUM_ERR:
            printf("Number must be valid natural integer\n");
            break;
        default:
            break;
        }
        return 1;
    }

    printf("%s\n", res);
    free(res);

    return 0;
}