#include <iostream>



class logical_values_array{
private:
    unsigned int value;
public:
    logical_values_array(unsigned int val = 0){
        value = val;
    }

    unsigned int get_val() const {
        return value;
    }
};



int main(){
    logical_values_array ar(3);
    std::cout << ar.get_val() << "\n";        



    return 0;
}