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
        return Complex(real_dif, imag_sum);
    }

    Complex divide(const Complex& other) const {
        double denominator = other.real * other.real + other.imag * other.imag;
        if (check_overflow(denominator)){
            throw overflow_error("Oveflow occured during operation");
        }
        if (denominator == 0) {
            throw invalid_argument("Division by zero in complex number");
        }
        double real_new = (real * other.real + imag * other.imag) / denominator;
        double imag_new = (imag * other.real - real * other.imag) / denominator;
        if (check_overflow(real_new) || check_overflow(imag_new)){
            throw overflow_error("Oveflow occured during operation");
        }
        return Complex(real_new, imag_new);
    }

    void display() const{
        cout << real << " + " << imag << "i" << endl;
    }

    double module() const { 
        double val = real * real + imag * imag;
        if (check_overflow(val)){
            throw overflow_error("Oveflow occured during operation");
        }
        return sqrt(val); 
    }

    double argument() const { 
        return atan2(imag, real);
    }
};



int main(){
    try{
        Complex c1(3, 4);    
        Complex c2(1, 2);   

        Complex sum = c1.add(c2);
        Complex diff = c1.subtract(c2); 
        Complex prod = c1.multiply(c2);
        Complex div = c1.divide(c2);  

        cout << "Sum: ";
        sum.display();
        cout << "Difference: "; 
        diff.display();
        cout << "Product: "; 
        prod.display();
        cout << "Division: "; 
        div.display();

        double arg = c1.argument();
        double mod = c1.module();

        Complex c3(-2, -2);    
        Complex c4(2, 2);   
        Complex c5(-2, 2);    
        Complex c6(0, -2);  
        cout << "Arguement of c3: " << c3.argument() << endl;
        cout << "Arguement of c4: " << c4.argument() << endl;
        cout << "Arguement of c5: " << c5.argument() << endl;
        cout << "Arguement of c6: " << c6.argument() << endl;
        cout << "Module of c1: " << mod << endl;
    } catch(const std::exception& e){
        cerr << e.what() << '\n';
    }

    return 0;
}