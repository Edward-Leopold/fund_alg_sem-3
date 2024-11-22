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

    unsigned int get_val() const {
        return value;
    }
};



int main(){
    logical_values_array a1(1);
    logical_values_array a2(2);
    logical_values_array a3 = a1.xor_mod(a2);


    std::cout << a1.get_val() << "\n";        
    std::cout << a2.get_val() << "\n";        
    std::cout << a3.get_val() << "\n";       

    return 0;
}