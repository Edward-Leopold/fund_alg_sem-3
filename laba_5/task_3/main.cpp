#include <iostream>


class logical_values_array{
private:
    unsigned int value;
public:
    logical_values_array(unsigned int val = 0){
        value = val;
    }

    logical_values_array inversion() const{
        return logical_values_array(~value);
    }

    logical_values_array conjunction(const logical_values_array& other) const{
        return logical_values_array(value & other.value);
    }

    logical_values_array disjunction(const logical_values_array& other) const{
        return logical_values_array(value | other.value);
    }

    logical_values_array implication(const logical_values_array& other) const{
        return logical_values_array(~value | other.value);
    }

    logical_values_array coimplication(const logical_values_array& other) const{
        return logical_values_array(value & ~other.value);
    }

    logical_values_array xor_mod(const logical_values_array& other) const{
        return logical_values_array(value ^ other.value);
    }

    logical_values_array equivalence(const logical_values_array& other) const{
        return logical_values_array(~(value ^ other.value));
    }

    logical_values_array pierce_arrow(const logical_values_array& other) const{
        return logical_values_array(~(value | other.value));
    }
    
    logical_values_array sheffer_stroke(const logical_values_array& other) const{
        return logical_values_array(~(value & other.value));
    }

    static bool equals(const logical_values_array& a, const logical_values_array& b){
        return a.value == b.value;
    }   

    bool get_bit(int position) const {
        if (position < 0 || position >= sizeof(int) * 8) {
            throw std::out_of_range("Bit position out of range");
        }
        return (value >> position) & 1;
    }

    void to_string(char* str, size_t size) const {
        if (str == nullptr){
            throw std::invalid_argument("Empty string is passed to method");
        }
        size_t len = sizeof(unsigned int) * 8;
        if (size < len + 1){
            throw std::invalid_argument("Not enough size of string");
        }
 
        for (int i = len - 1, j = 0; i >= 0; --i, j++) {
            str[j] = ((value >> i) & 1) + '0';
        }
        str[len] = '\0';
    }

    unsigned int get_val() const {
        return value;
    }
};



int main(){
    try{
        logical_values_array a1(1);
        logical_values_array a2(2);
        logical_values_array a3 = a1.xor_mod(a2);

        std::cout << "a1: " << a1.get_val() << "\n";        
        std::cout << "a2: " << a2.get_val() << "\n";        
        std::cout << "conjunction: " << a1.conjunction(a2).get_val() << "\n";       
        std::cout << "disjunction: " << a1.disjunction(a2).get_val() << "\n";       
        std::cout << "implication: " << a1.implication(a2).get_val() << "\n";       
        std::cout << "coimplication: " << a1.coimplication(a2).get_val() << "\n";       
        std::cout << "xor_mod: " << a1.xor_mod(a2).get_val() << "\n";       
        std::cout << "equivalence: " << a1.equivalence(a2).get_val() << "\n";       
        std::cout << "pierce_arrow: " << a1.pierce_arrow(a2).get_val() << "\n";       
        std::cout << "sheffer_stroke: " << a1.sheffer_stroke(a2).get_val() << "\n";       

        std::cout << "Equals: " << (logical_values_array::equals(a1, a2) ? "yes" : "no") << "\n";       
        std::cout << "0 bit of a1: " << a1.get_bit(0) << "\n"; 
        std::cout << "1 bit of a1: " << a1.get_bit(1) << "\n"; 

        char str[50];
        a1.to_string(str, 43);
              
        std::cout << "a1 to string: " << str << "\n"; 
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
    }

    return 0;
}