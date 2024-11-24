#include <iostream>


class Product{
protected:
    std::string name;
    unsigned int id;
    double weight;
    double price;
    int period;
public:
    Product(const std::string &name, unsigned int id, double weight, double price, int storagePeriod)
            : name(name), id(id), weight(weight), price(price), period(storagePeriod) {
        if (weight < 0 || price < 0 || storagePeriod < 0) {
            throw std::invalid_argument("Weight, price, and storage period must be non-negative.");
        }
    }

    virtual double calculateStorageFee() const { return weight * 100; }

    virtual void displayInfo() const {
        std::cout << 
            "Name: " << name << "\n" <<
            "ID: " << id << "\n" <<
            "Weight: " << weight << " kg" << "\n" << 
            "Price: $" << price << "\n" << 
            "Storage Period: " << period << " days" << "\n";
    }
};

class PerishableProduct: public Product{
private:
    int expirationDate;
public:
    PerishableProduct(const std::string &name, unsigned int id, double weight, double price, int storagePeriod, int expirationDate): 
    Product(name, id, weight, price, storagePeriod), expirationDate(expirationDate){
         if (expirationDate < 0){
            throw std::invalid_argument("Can't store expired product!");
         }
    }
    double calculateStorageFee() const override{

    }
};

class ElectronicProduct: public Product{
private:
    int warrantyPeriod;
    double powerRating;
public:
    ElectronicProduct(const std::string &name, unsigned int id, double weight, double price, int storagePeriod, int warrantyPeriod, double powerRating): 
    Product(name, id, weight, price, storagePeriod), warrantyPeriod(warrantyPeriod), powerRating(powerRating){
        if (warrantyPeriod < 1){
            throw std::invalid_argument("Warranty period can't be less than 1 day");
        }
        if (powerRating < 0){
            throw std::invalid_argument("Power rating must be non-negative");
        }
    }
    void displayInfo() const override{
        Product::displayInfo();
        std::cout << "Warranty period: " << warrantyPeriod << " days" << "\n"
        << "Power rating: " << powerRating << " W" << "\n" ;
    }
};

class BuildingMaterial: public Product{
private:
    bool flammability;
public:
    BuildingMaterial(const std::string &name, unsigned int id, double weight, double price, int storagePeriod, bool flammability): 
    Product(name, id, weight, price, storagePeriod), flammability(flammability) {}

    double calculateStorageFee() const override{

    }
};


class Warehouse{
    private:

    public:

}; 




int main(){
    try{
        
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
    }

    return 0;
}