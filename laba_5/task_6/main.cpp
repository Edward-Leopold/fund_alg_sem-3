#include <iostream>
#include <vector>
#include <cstring>

class vector{
private:
    double* v_data;
    std::size_t v_size;
    std::size_t v_capacity;
public:
    vector(const size_t n, const double &init_val = 0.0){
        v_size = n;
        v_capacity = n;
        v_data = new double[n];
        for (int i = 0; i < n; i++){
            v_data[i] = init_val;
        }
    }

    vector(std::vector<double>::iterator start, std::vector<double>::iterator end){
        if (start >= end) {
            throw std::invalid_argument("Invalid iterators passed to constructor");
        }
        v_size = end - start;
        v_capacity = v_size;
        v_data = new double[v_size];
        std::copy(start, end, v_data);
    }

    vector(std::vector<double>::const_iterator start, std::vector<double>::const_iterator end){
        if (start >= end) {
            throw std::invalid_argument("Invalid iterators passed to constructor");
        }
        v_size = end - start;
        v_capacity = v_size;
        v_data = new double[v_size];
        std::copy(start, end, v_data);
    }

    vector(std::initializer_list<double> init) {
        v_size = init.size();
        v_capacity = v_size;
        v_data = new double[v_size];
        std::copy(init.begin(), init.end(), v_data);
    }

    double& at(size_t index) const { // const double& ?????????
        if (index >= v_size){
            throw std::invalid_argument("Incorrect index passed");
        }
        return v_data[index];
    }

    double& front() const {
        if (v_size == 0) {
            throw std::runtime_error("Can't get front element of empty vector");
        }
        return v_data[0];
    }

    double& back() const {
        if (v_size == 0) {
            throw std::runtime_error("Can't get back element of empty vector");
        }
        return v_data[v_size - 1];
    }

    double* data() const {
        return v_data;
    }

    bool empty() const {
        return (v_size == 0) ? true : false;
    }

    size_t size() const{
        return v_size;
    }

    size_t capacity() const{
        return v_capacity;
    }

    void reserve(size_t num) {
        if (num > v_capacity){
            double* new_data = new double[num];
            std::memset(new_data, 0, num * sizeof(double));
            std::memcpy(new_data, v_data, v_size * sizeof(double));
            delete[] v_data;
            v_capacity = num;
            v_data = new_data;    
        }
    }

    void shrink_to_fit() {
        if (v_capacity > v_size){
            double* shrinked_data = new double[v_size];
            std::memcpy(shrinked_data, v_data, v_size * sizeof(double));
            delete[] v_data;
            v_data = shrinked_data;
            v_capacity = v_size;
        }
    }

    void clear() {
        v_size = 0;
    }

    void insert(size_t index, double elem){
        if (index > v_size) {
            throw std::invalid_argument("Passed index value must be in range of v_size of vector");
        }
        if (v_capacity == v_size){
            reserve(v_capacity + 100);
        }

        v_size += 1;
        for (size_t i = v_size; i != index; i--){
            v_data[i] = v_data[i - 1];
        }
        v_data[index] = elem;
    } 

    ~vector(){
        delete[] v_data;
    }
    
};


int main(){
    try{
        vector v{1, 2, 3, 4, 5, 6, 7, 8, 9};
        double* data = v.data();

        for (int i = 0; i < v.size(); i++) std::cout << data[i] << " ";
        std::cout << '\n';

        v.insert(4, 12.9023);

        data = v.data();
        for (int i = 0; i < v.size(); i++) std::cout << data[i] << " ";
        std::cout << '\n';
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
    }

    return 0;
}