#include <iostream>
#include <cmath>

using namespace std;

class Complex{
private:
    double real;
    double imag;

    static bool check_overflow(const double num){
        return (!isfinite(num) || isnan(num));
    }
public:

    Complex(double c_real = 0, double c_imag = 0){
        real = c_real;
        imag = c_imag;
        cout << "an object of complex object has been created: " << real << " + " << imag << "i" <<"\n";
    }

    Complex add(const Complex& other) const {
        double real_sum = real + other.real;
        double imag_sum = imag + other.imag;
        if (check_overflow(real_sum) || check_overflow(imag_sum)){
            throw overflow_error("Oveflow occured during operation");
        }
        return Complex(real_sum, imag_sum);
    }

    Complex subtract(const Complex& other) const {
        double real_sub = real - other.real;
        double imag_sub = imag - other.imag;
        if (check_overflow(real_sub) || check_overflow(imag_sub)){
            throw overflow_error("Oveflow occured during operation");
        }
        return Complex(real_sub, imag_sub);
    }

    Complex multiply(const Complex& other) const {
        double real_dif = real * other.real - imag * other.imag;
        double imag_sum = real * other.imag + imag * other.real;
        if (check_overflow(real_dif) || check_overflow(imag_sum)){
            throw overflow_error("Oveflow occured during operation");
        }
        return Complex(
            real_dif,
            imag_sum
        );
    }

    Complex divide(const Complex& other) const {
        double denominator = other.real * other.real + other.imag * other.imag;
        if (denominator == 0) {
            throw invalid_argument("Division by zero in complex number");
        }
        return Complex(
            (real * other.real + imag * other.imag) / denominator,
            (imag * other.real - real * other.imag) / denominator
        );
    }

    void display() const{
        cout << real << " + " << imag << "i" << endl;
    }
};



int main(){
    Complex c;
    

    return 0;
}