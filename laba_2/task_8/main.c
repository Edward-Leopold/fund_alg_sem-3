#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef enum errCodes{
    MALLOC_ERR,
    REALLOC_ERR,
    BASE_ERR,
    NUM_IN_BASE_ERR,

    SUCCESS
}errCodes;

int get_len(const char* arr){
    int temp = 0;
    for (int i = 0; arr[i]; i++) temp++;
    return temp;
}

int to_int(const char ch){
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'z') return ch - 'a' + 10;
    return -1;
}

char to_char(const int n){
    if (n >= 0 && n <= 9) return n + '0';
    if (n >= 10 && n <= 35) return n + 'a' - 10;
    return 0;
}

int reverse(const char* str, char** reversed){
    int len = get_len(str);
    for (int i = 0; i < len; i++) (*reversed)[i] = str[len - i - 1];
    (*reversed)[len] = '\0';
    return 0; 
}

errCodes sum(const int base, const char* a, const char* b, char** result){
    int len_a = get_len(a);
    int len_b = get_len(b);

    char* reversed_sum = malloc(sizeof(char) * (MAX(len_a, len_b) + 2));
    if (!reversed_sum) return MALLOC_ERR;
    int count = 0;

    char* reversed_a = malloc(sizeof(char) * (len_a + 1));
    char* reversed_b = malloc(sizeof(char) * (len_b + 1));
    if (!reversed_a || !reversed_b) {
        free(reversed_sum);
        free(reversed_a);
        free(reversed_b);
        return MALLOC_ERR;
    }

    reverse(a, &reversed_a);
    reverse(b, &reversed_b);

    int over = 0;
    for (int i = 0; i < MIN(len_a, len_b); i++){
        int num_a = to_int(reversed_a[i]);
        int num_b = to_int(reversed_b[i]);

        if (num_a >= base || num_b >= base || num_a == -1 || num_b == -1){ 
            free(reversed_sum);
            free(reversed_a);
            free(reversed_b);
            return NUM_IN_BASE_ERR;
        }

        int res = num_a + num_b + over;
        if (res >= base) {
            over = res / base;
            res = res - base;
        } else{
            over = 0;
        }
        reversed_sum[count] = to_char(res);
        count++;
    }

    if(len_a == len_b && over > 0) {
        reversed_sum[count] = to_char(over);
        count++;
    }

    char* reversed_max = (len_a > len_b) ? reversed_a : reversed_b;
    for (int i = count; i < MAX(len_a, len_b); i++){
        int num = to_int(reversed_max[i]);

        if (num >= base || num == -1) {
            free(reversed_sum);
            free(reversed_a);
            free(reversed_b);
            return NUM_IN_BASE_ERR;
        }

        int res = num + over;
        over = 0;
        reversed_sum[count] = to_char(res);
        count++;
    }

    reversed_sum[count] = '\0';

    char* summary = malloc(sizeof(char) * (count + 1));
    if (!summary) {
        free(reversed_sum);
        free(reversed_a);
        free(reversed_b);
        return MALLOC_ERR;
    }
    reverse(reversed_sum, &summary);

    *result = summary;

    free(reversed_sum);
    free(reversed_a);
    free(reversed_b);

    return SUCCESS;
}

errCodes sum_all(char** result, const int base, const int count, ...){
    if (base < 2 || base > 36) return BASE_ERR;

    char *output = malloc(2 * sizeof(char));
    if (!output) return MALLOC_ERR;
    output[0] = '0';
    output[1] = '\0';

    va_list nums;
    va_start(nums, count);
    for (int i = 0; i < count; i++){
        char* num = va_arg(nums, char*);
        errCodes err_status = sum(base, output, num, &output);
        if (err_status != SUCCESS) {
            va_end(nums);
            free(output);
            return err_status;
        }
    }
    va_end(nums);

    *result = output;

    return SUCCESS;
}

int main(int argc, char** argv){

    char *res;
    errCodes status = sum_all(&res, 6, 3, "10", "231", "345");
    if (status != SUCCESS){
        switch (status){
        case MALLOC_ERR:
            printf("Malloc error occured!\n");
            break;
        case REALLOC_ERR:
            printf("Realloc error occured!\n");
            break;
        case BASE_ERR:
            printf("The base of numbers must be from 2 to 36\n");
            break;
        case NUM_IN_BASE_ERR:
            printf("All passed numbers must be natural and correspond to the passed base \n");
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