#include <iostream>


class Product
{
private:
    std::string name;
    unsigned int id;
    double weight;
    double price;
    int expiration;
public:
    Product(const std::string &name, unsigned int id, double weight, double price, int storagePeriod)
            : name(name), id(id), weight(weight), price(price), expiration(storagePeriod) {
        if (weight < 0 || price < 0 || storagePeriod < 0) {
            throw std::invalid_argument("Weight, price, and storage period must be non-negative.");
        }
    }

    virtual double calculateStorageFee() const = 0;

    virtual void displayInfo() const {
        std::cout << 
            "Name: " << name << "\n" <<
            "ID: " << id << "\n" <<
            "Weight: " << weight << " kg" << "\n" << 
            "Price: $" << price << "\n" << 
            "Storage Period: " << expiration << " days" << "\n";
    }
};



int main(){
    try{
        
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
    }

    return 0;
}