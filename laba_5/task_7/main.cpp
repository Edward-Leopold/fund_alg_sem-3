#include <iostream>


class Product
{
private:
    std::string name;
    unsigned int id;
    double weight;
    double price;
    unsigned int expiration;
public:
    Product(/* args */);

};



int main(){
    try{
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}