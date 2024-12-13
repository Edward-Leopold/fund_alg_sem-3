#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

typedef enum errCodes{
    SUCCESS,

    MALLOC_ERR,
    REALLOC_ERR,
    BASE_ERR,
    NUM_IN_BASE_ERR,
	NO_NUMBERS_KAPREKAR,

}errCodes;

double fast_pow(double base, int exp) {
    double result = 1.0;
    while (exp > 0) {
        if (exp % 2 == 1) result *= base;
        base *= base;
        exp /= 2;
    }
    return result;
}

int charToInt(char c){
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
	if (c >= 'a' && c <= 'z') return c - 'a' + 10;
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

long long gorner(const char *number, int base) {
    long long res = 0;
    int digit;
    for (int i = 0; number[i]; ++i){
        digit = charToInt(number[i]);
        if (digit < 0 || digit >= base){
            return -1;
        }
        res = res * base + digit;
    }
    
    return res;
}

char* back_gorner(long long num, int base){
    int capacity = 10;
    int size = 0;
    char* res = (char*)malloc(sizeof(char) * capacity);
    if (!res) return NULL;

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
    res[size] = '\0';

    reverseString(res, size);
    return res;
}

errCodes split_string(const char *input, int split_index, char **part1, char **part2) {
    int len = strlen(input);

    if (split_index < 0 || split_index > len) {
        return MALLOC_ERR;
    }

    *part1 = (char *)malloc((split_index + 1) * sizeof(char));
    if (*part1 == NULL) {
        return MALLOC_ERR;
    }

    *part2 = (char *)malloc((len - split_index + 1) * sizeof(char));
    if (*part2 == NULL) {
        free(*part1);
        return MALLOC_ERR;
    }

    strncpy(*part1, input, split_index);
    (*part1)[split_index] = '\0'; 

    strcpy(*part2, input + split_index);

    return SUCCESS;
}

int get_direction(double a_x, double a_y, double b_x, double b_y, double c_x, double c_y) {
	double ab_x = b_x - a_x;
	double ab_y = b_y - a_y;
	double bc_x = c_x - b_x;
	double bc_y = c_y - b_y;
	double prod = ab_x * bc_y - ab_y * bc_x;
	if (prod > 0) {
		return 1;
	} else if (prod < 0) {
		return -1;
	}
	return 0;
}

int is_convex(int points_cnt, ...) {
	if (points_cnt < 3) return 0;

	va_list points;
	va_start(points, points_cnt);
	double x1 = va_arg(points, double);
	double y1 = va_arg(points, double);
	double x2 = va_arg(points, double);
	double y2 = va_arg(points, double);
	double x3 = va_arg(points, double);
	double y3 = va_arg(points, double);

    double first_x = x1;
    double first_y = y1;
    double second_x = x2;
    double second_y = y2;

	int init_direction = get_direction(x1, y1, x2, y2, x3, y3);

	if (init_direction == 0){
		va_end(points);
		return 0;
	}

	for (int i = 3; i < points_cnt; ++i) {
		x1 = x2;
		y1 = y2;
		x2 = x3;
		y2 = y3;
		x3 = va_arg(points, double);
		y3 = va_arg(points, double);

		int dir = get_direction(x1, y1, x2, y2, x3, y3);

		if (dir == 0) {
			va_end(points);
			return 0;
		}
		if (dir != init_direction) {
			va_end(points);
			return 0;
		}
	}
	va_end(points);

    int last1 = get_direction(x2, y2, x3, y3, first_x, first_y);
    int last2 = get_direction(x3, y3, first_x, first_y, second_x, second_y);
    int result = (last1 == init_direction && last2 == init_direction); 

	return result;
}

double polynomial(double point, int degree, ...){
    double sum = 0.0;

    va_list coeffs;
    va_start(coeffs, degree);
    for (int n = degree; n >= 0; --n){
        double coef = va_arg(coeffs, double);
        double res = fast_pow(point, n) * coef;
        sum += res;
    }
    va_end(coeffs);
    return sum;
}

errCodes isKaprekar(long long num, int base, int* result){
	if (num == 1) {
		*result = 1;
		return SUCCESS;
	}

	*result = 0;
	long long square = num * num;
	char* square_str = back_gorner(square, base);
	if (!square_str){
		return MALLOC_ERR;
	}

	int len = strlen(square_str);
	char* left_substr = NULL;
	char* right_substr = NULL;
	for (int i = 1; i < len; i++) {
		errCodes split_status = split_string(square_str, i, &left_substr, &right_substr);
		if (split_status != SUCCESS){
			free(square_str);
			return split_status;
		}
		
		int left = gorner(left_substr, base);
		int right = gorner(right_substr, base);

		if (left == -1 || right == -1){
			free(left_substr);
			free(right_substr);
			free(square_str);
			return NUM_IN_BASE_ERR;
		}

		if (right > 0 && left + right == num) {
			*result = 1;
		}
	}
	free(left_substr);
	free(right_substr);
	free(square_str);

	return SUCCESS;
}

errCodes find_kaprekar(char*** result, int base, int nums_count, ...){
	if (base < 0 || base > 36){
		return BASE_ERR;
	}

	if (nums_count <= 0){
		return NO_NUMBERS_KAPREKAR;
	}

	va_list nums;
	va_start(nums, nums_count);
	int capacity = 10;
	int size = 0;
	char** kaps_nums = (char**)malloc(sizeof(char*) * capacity);
	if (!kaps_nums){
		va_end(nums);
		return MALLOC_ERR;
	}

	for(int i = 0; i < nums_count; ++i){
		const char *str_num = va_arg(nums, const char *);
		long long decimal_num = gorner(str_num, base);
		if (decimal_num < 0) {
			free(kaps_nums);
			va_end(nums);
			return NUM_IN_BASE_ERR;
		}
		
		int is_kapr;
		errCodes kapr_status = isKaprekar(decimal_num, base, &is_kapr);
		if (kapr_status != SUCCESS){
			free(kaps_nums);
			va_end(nums);
			return kapr_status;
		}

		if (is_kapr){
			if (size == capacity - 1){
				capacity *= 2;
				char** temp = (char**)realloc(kaps_nums, sizeof(char*) * capacity);
				if (!temp){
					free(kaps_nums);
					va_end(nums);
				}
				kaps_nums = temp;
			}

			char* copy = strdup(str_num);
			if (!copy) {
				free(kaps_nums);
				va_end(nums);
				return MALLOC_ERR;
			}

			kaps_nums[size++] = copy;
		}
	}
	kaps_nums[size] = NULL;
	va_end(nums);

	*result = kaps_nums;

	return SUCCESS;
}

int main(int argc, char** argv){

	int convex1 = is_convex(3, -3.0, 2.0, -2.0, -1.0, 3.0, 2.0);
	if (convex1) printf("Выпуклый\n");
	else printf("Не выпуклый\n");

    int convex2 = is_convex(5, 6.0, 4.0, 10.0, 4.0, 15.0, 10.0, 6.0, 8.0, 10.0, 6.0); 
	if (convex2) printf("Выпуклый\n");
	else printf("Не выпуклый\n");

	int convex3 = is_convex(3, 6.0, 6.0, 6.0, 6.0, 6.0, 6.0); 
	if (convex3) printf("Выпуклый\n");
	else printf("Не выпуклый\n");

	int convex4 = is_convex(3, 2.0, 3.0, 3.0, 4.0, 2.0, 3.0); 
	if (convex4) printf("Выпуклый\n");
	else printf("Не выпуклый\n");

	// 12*x^3 - 85*x^2 + 34.8*x - 7.005
	double polynominal_res = polynomial(2, 3, 12.0, -85.0, 34.8, -7.005);
	printf("result of polynominal: %lf\n", polynominal_res);

	double polynominal_res2 = polynomial(200, 3, 0.0, 0.0, 0.0, 0.0);
	printf("result of polynominal: %lf\n", polynominal_res2);

	// char* str = "123456789";
	// for (int i = 1; i < strlen(str); ++i){
	// 	char* left; 
	// 	char* right;
	// 	split_string(str, i, &left, &right);
	// 	printf("%s %s\n", left, right);
	// 	free(left);
	// 	free(right);
	// }



	char** kaps = NULL;
	int base = 10;
	errCodes kapStatus = find_kaprekar(&kaps, base, 6, "1", "9", "45", "55", "98", "297");
	if (kaps[0]){
		printf("Найденные числа Капрекара по основанию %d: \n", base);
	} else{
		printf("В переданной последовательности чисел не найдено чисел Капрекара\n");
	}
	for (int i = 0; kaps[i]; ++i){
		printf("%s\n", kaps[i]);
		free(kaps[i]);
	}
	free(kaps);

    return 0;
}