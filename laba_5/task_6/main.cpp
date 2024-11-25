#include <iostream>
#include <vector>


class vector{
private:
    double* data;
    std::size_t size;
    std::size_t capacity;
public:
    vector(const size_t n, const double &init_val = 0.0){
        size = n;
        capacity = n;
        data = new double[n];
        for (int i = 0; i < n; i++){
            data[i] = init_val;
        }
    }

    vector(std::vector<double>::iterator start, std::vector<double>::iterator end){
        if (start >= end) {
            throw std::invalid_argument("Invalid iterators passed to constructor");
        }
        size = end - start;
        capacity = size;
        data = new double[size];
        std::copy(start, end, data);
    }

    vector(std::vector<double>::const_iterator start, std::vector<double>::const_iterator end){
        if (start >= end) {
            throw std::invalid_argument("Invalid iterators passed to constructor");
        }
        size = end - start;
        capacity = size;
        data = new double[size];
        std::copy(start, end, data);
    }

    vector(std::initializer_list<double> init) {
        size = init.size();
        capacity = size;
        data = new double[size];
        std::copy(init.begin(), init.end(), data);
    }
    
};


int main(){
    try{
        std::vector<int> vec ( 3, 3);
        for (const auto some: vec){
            std::cout << some << " ";
        }
        std::cout << "\n";
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
    }

    return 0;
}