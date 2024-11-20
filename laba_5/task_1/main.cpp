#include <iostream>

using namespace std;

class Complex{
    
public:
    double real;
    double imag;

    Complex(double c_real = 0, double c_imag = 0){
        real = c_real;
        imag = c_imag;
        cout << "an object of complex object has been created: " << real << " + " << imag << "i" <<"\n";
    }

    Complex add(const Complex& other) const {
        return Complex(real + other.real, imag + other.imag);
    }

    Complex subtract(const Complex& other) const {
        return Complex(real - other.real, imag - other.imag);
    }

    Complex multiply(const Complex& other) const {
        return Complex(
            real * other.real - imag * other.imag,
            real * other.imag + imag * other.real
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