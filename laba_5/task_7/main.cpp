#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>


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

    unsigned int getId() const { return id; }
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
        return 1;
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
        return 1;
    }
};


class Warehouse{
private:
    std::vector<std::unique_ptr<Product>> products;
public:
    void addProduct(Product* product) {
        if (!product) {
            throw std::invalid_argument("Cannot add null product to warehouse.");
        }
        products.push_back(std::unique_ptr<Product>(product));
    }

    void removeProduct(unsigned int id) {
        auto iter = std::remove_if(products.begin(), products.end(), [id](const auto &p) { return p->getId() == id; });
        if (iter == products.end()) {
            throw std::runtime_error("Product with ID " + std::to_string(id) + " not found.");
        }
        products.erase(iter, products.end());
    }

    void displayAllProducts() const {
        for (const auto &product: products) {
            product->displayInfo();
            std::cout << "--------------------\n";
        }
    }

}; 




int main(){
    try{
        Warehouse ware;
        
        PerishableProduct p1{"kolbasa", 12345, 0.4, 300, 12, 50};
        PerishableProduct p2{"hleb", 819345, 0.4, 80, 10, 7};
        ware.addProduct(&p1);
        ware.addProduct(&p2);

        ware.displayAllProducts();


    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
    }

    return 0;
}