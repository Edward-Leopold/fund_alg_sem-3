#include <stdio.h>
#include <math.h>


double ln2_equ(double x){
    return x * x - 2;
}

double exp_equ(double x){
    return pow(x - 1.5, 2) / (x - 1.5);
}

double dichotomy(double left, double right, double eps, double (*f)(double)){
    double center = 1;
    while(fabs(right - left) > eps){
        center = (right + left) / 2.0;
        if(f(left) * f(center) > 0){
            left = center;
        } else{
            right = center;
        }
    }
    return center;
}

int main(int argc, char** argv){
    double eps1 = 0.0000000000001;
    double eps2 = 2;
    double eps3 = 0.00000000001;

    printf("Equation 1 for eps = %.10lf: %.10lf\n", eps1, dichotomy(0.0, 2.0, eps1, ln2_equ));
    printf("Equation 1 for eps = %.10lf: %.10lf\n", eps1, dichotomy(2.0, 3.0, eps1, ln2_equ));
    printf("Equation 1 for eps = %.10lf: %.10lf\n", eps1, dichotomy(0.0, -1.0, eps1, ln2_equ));
    printf("Equation 1 for eps = %.10lf: %.10lf\n", eps2, dichotomy(0.0, 1.0, eps2, ln2_equ));
    printf("Equation 1 for eps = %.10lf: %.10lf\n", eps3, dichotomy(0.0, 1.0, eps3, ln2_equ));
    printf("\n");
    printf("Equation 2 for eps = %.10lf: %.10lf\n", eps1, dichotomy(0.0, 2.1111, eps1, exp_equ));
    printf("Equation 2 for eps = %.10lf: %.10lf\n", eps1, dichotomy(11.0, 12.0, eps1, exp_equ));
    printf("Equation 2 for eps = %.10lf: %.10lf\n", eps1, dichotomy(0.0, 2.0, eps1, exp_equ));
    printf("Equation 2 for eps = %.10lf: %.10lf\n", eps2, dichotomy(3.0, 50000.0, eps2, exp_equ));
    printf("Equation 2 for eps = %.10lf: %.10lf\n", eps3, dichotomy(5.0, 150.0, eps3, exp_equ));

    return 0;
}