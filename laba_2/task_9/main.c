#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>

void convert(int base, int count, ...){
    va_list nums;
    va_start(nums, count);
    
    for(int i = 0; i < count; i++){
        double n = va_arg(nums, double);
        double orig = n;
        double converted = 0;

        char* conv_nums = malloc(sizeof(char) * 2);
        if (conv_nums == NULL) return;
        int digits_index = 0;
        int malloc_digits = 1;

        while(n){
            n = base * n;
            if(!isfinite(n)) break;
            if (digits_index + 1 > malloc_digits){
                malloc_digits *= 2;
                conv_nums = realloc(conv_nums, sizeof(char) * (malloc_digits + 1));
            }
            conv_nums[digits_index] = (int)n + '0';
            digits_index++;
            // converted = converted / 10 * (int)n;
            n = n - (int)n;
        }
        conv_nums[digits_index + 1] = '\0';
         printf("Converted number in base %d: ", base);
        for(int i = 0; conv_nums[i]; i++) {
            printf("%c", conv_nums[i]);
        }
        printf("\n");
        printf("converted %lf to base %d = %.10lf\n", orig, base, converted);
    }
    va_end(nums);
}

int main(){
    convert(2, 1, 0.125);
    return 0;
}