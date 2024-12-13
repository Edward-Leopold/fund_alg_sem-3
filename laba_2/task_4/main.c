#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

typedef enum errCodes{
    MALLOC_ERR,
    REALLOC_ERR,
    BASE_ERR,
    NUM_IN_BASE_ERR,

    SUCCESS
}errCodes;

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

    return 0;
}