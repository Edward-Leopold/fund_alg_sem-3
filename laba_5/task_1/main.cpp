#include <iostream>

using namespace std;

int decrement(int num) {
    long int mask = 1;
    while ((num & mask) == 0) {
        num = num ^ mask;
        mask <<= 1;
    }
    return num ^ mask;  
}

int increment(int num) {
    int mask = 1;
    while (num & mask) {
        num = num ^ mask;
        mask <<= 1;
    }
    return num ^ mask; 
}

class binary_int{
private:
    int n;

    

public:
    binary_int(int val){
        n = val;
    }

    binary_int operator - () const {
        return binary_int(increment(~n));
    }
    // prefix operators
    binary_int& operator++ (){
        n = increment(n);
        return *this;
    }
    binary_int& operator-- (){
        n = decrement(n);
        return *this;
    }
    // postfix operators
    binary_int operator++ (int){
        binary_int temp(*this);
        n = increment(n);
        return temp;
    }
    binary_int operator-- (int){
        binary_int temp(*this);
        n = decrement(n);
        return temp;
    }

    binary_int operator + (const binary_int& other) const{
        int res = n;
        int other_n = other.n;
        if(other_n > 0) {
            while (other_n > 0) {
                res = increment(res);
                other_n = decrement(other_n);
            }
        }else{
            while (other_n < 0) {
                res = decrement(res);
                other_n = increment(other_n);
            }
        }
        return binary_int(res);
    }
    binary_int operator += (const binary_int& other){
        *this = *this + other;
        return *this;
    }

    binary_int operator - (const binary_int& other) const{
        binary_int res = *this + (-other);
        return res;
    }
    binary_int operator -= (const binary_int& other){
        *this = *this - other;
        return *this;
    }

    binary_int operator * (const binary_int& other) const{
        binary_int res(0);
        if (n == 0 || other.n == 0) return res;
        int cnt = other.n;
        if (cnt > 0){
            while (cnt > 0){
                res += *this;
                cnt = decrement(cnt);
            }
        } else if(cnt < 0){
            while (cnt < 0){
                res -= *this;
                cnt = increment(cnt);
            }
        }
        return res;
    }
    binary_int operator *= (const binary_int& other){
        *this = *this * other;
        return *this;
    }

    binary_int operator <<= (const binary_int& other){
        n <<= other.n;
        return *this;
    }
    binary_int operator >>= (const binary_int& other){
        n >>= other.n;
        return *this;
    }
    binary_int operator << (const binary_int& other) const{
        int res = n << other.n;
        return binary_int(res);
    }
    binary_int operator >> (const binary_int& other) const{
        int res = n >> other.n;
        return binary_int(res);
    }

    int get_val() const{
        return n;
    }
};

ostream& operator << (ostream &os, const binary_int &b){
    return os << b.get_val();
}


int main(){
    // int a = 0;
    // cout << decrement(0) << "\n";
    

    binary_int b1(3);
    binary_int b2(5);
    binary_int b3 = b1 * b2;
    binary_int b4(-2);

    cout << "b1: " << b1 << "\n";
    cout << "b2: " << b2 << "\n";
    cout << "b3: " << b3 << "\n";
    cout << "b4: " << b4 << "\n";
    b4 *= -b3;

    cout << "b1: " << b1 << "\n";
    cout << "b2: " << b2 << "\n";
    cout << "b3: " << b3 << "\n";
    cout << "b4: " << b4 << "\n";

    

    return 0;
}