#include <iostream>
#include <vector>
#include <cstring>

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

    double& at(size_t index) const { // const double& ?????????
        if (index >= size){
            throw std::invalid_argument("Incorrect index passed");
        }
        return data[index];
    }

    double& front() const {
        if (size == 0) {
            throw std::runtime_error("Can't get front element of empty vector");
        }
        return data[0];
    }

    double& back() const {
        if (size == 0) {
            throw std::runtime_error("Can't get back element of empty vector");
        }
        return data[size - 1];
    }

    double* data() const {
        return data;
    }

    bool empty() const {
        return (size == 0) ? true : false;
    }

    size_t size() const{
        return size;
    }

    size_t capacity() const{
        return capacity;
    }

    void reserve(size_t num) {
        if (num > capacity){
            double* new_data = new double[num];
            std::memset(new_data, 0, num * sizeof(double));
            std::memcpy(new_data, data, size * sizeof(double));
            delete[] data;
            capacity = num;
            data = new_data;    
        }
    }

    void shrink_to_fit() {
        if (capacity > size){
            double* shrinked_data = new double[size];
            std::memcpy(shrinked_data, data, size * sizeof(double));
            delete[] data;
            data = shrinked_data;
            capacity = size;
        }
    }

    void clear() {
        size = 0;
    }

    void insert(size_t index, double elem){
        if (index > size) {
            throw std::invalid_argument("Passed index value must be in range of size of vector");
        }
        if (capacity < size + 1){
            reserve(capacity + 100);
        }
        for (size_t i = size - 1; i != index; i--){
            data[i + 1] = data[i];
        }
        data[index] = elem;
        size += 1;
    } 

    ~vector(){
        delete[] data;
    }
    
};


int main(){
    try{
        vector v{1, 2, 3, 4, 5, 6, 7, 8, 9};
        // double* data = v.capacity();
        
        
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
    }

    return 0;
}