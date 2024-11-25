#include <iostream>
#include <vector>
#include <cstring>
#include <compare>

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

    ~vector(){
        delete[] v_data;
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

    void push_back(double elem){
        if (v_capacity == v_size){
            reserve(v_capacity + 100);
        }
        
        v_data[v_size] = elem;
        v_size += 1;
    }

    void insert(size_t index, double elem){
        if (index > v_size) {
            throw std::invalid_argument("Passed index value must be in range of size of vector");
        }
        if (index == v_size) {
            push_back(elem);
            return;
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

    void erase(size_t index){
        if (v_size == 0){
            throw std::invalid_argument("The vector is empty, nothing to erase");
        }
        if (index > v_size - 1) {
            throw std::invalid_argument("Passed index value must be in range of size of vector");
        }

        for (size_t i = index; i < v_size - 1; i++){
            v_data[i] = v_data[i + 1];
        }
        v_size -= 1;
    }

    void pop_back(){
        if (v_size == 0){
            throw std::runtime_error("The vector is empty, nothing to pop");
        }
        v_size -= 1;
    }

    void resize(size_t size, double elem){
        if (size > v_size){
            reserve(size);
            for (int i = v_size; i < size; i++){
                v_data[i] = elem;
            }
            v_size = size;
        }
    }

    std::weak_ordering operator <=> (const vector &other) const {
        if (other.v_size != v_size) {
            if (v_size < other.v_size) {
                return std::weak_ordering::less;
            } else {
                return std::weak_ordering::greater;
            }
        }
        for (int i = 0; i < v_size; i++) {
            if (v_data[i] != other.v_data[i]) {
                if (v_data[i] > other.v_data[i]) {
                    return std::weak_ordering::greater;
                } else {
                    return std::weak_ordering::less;
                }
            }
        }
        return std::weak_ordering::equivalent;
    }

    bool operator == (const vector& other) const {
        if (v_size != other.v_size) {
            return false;
        }
        for (size_t i = 0; i < v_size; i++){
            if (v_data[i] != other.v_data[i]){
                return false;
            }
        }
        return true;
    }
    
};


int main(){
    try{
        vector v{1, 2, 3, 4, 5, 6, 7, 8, 9};
        vector v2(16, 2);
        double* data = v.data();

        for (int i = 0; i < v.size(); i++) std::cout << data[i] << " ";
        std::cout << '\n';

        v.insert(4, 12.9023);
        v.push_back(0.99);
        v.push_back(0.98);
        v.push_back(0.97);

        data = v.data();
        for (int i = 0; i < v.size(); i++) std::cout << data[i] << " ";
        std::cout << '\n';

        // v.resize(140, 36.6);
        v.erase(4);
        data = v.data();
        for (int i = 0; i < v.size(); i++) std::cout << data[i] << " ";
        std::cout << '\n';

        std::cout << (v > v2) << std::endl;

    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
    }

    return 0;
}