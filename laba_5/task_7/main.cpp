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
            "Price: " << price << " rub" << "\n" << 
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
        double extra = (expirationDate > 5) ? 1.0 : ((6 - expirationDate) * 0.05) + 1;
        double res = Product::calculateStorageFee() * extra;
        return res;
    }

    int getDate() const {
        return expirationDate;
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
    int flammability;
public:
    BuildingMaterial(const std::string &name, unsigned int id, double weight, double price, int storagePeriod, int flammability): 
    Product(name, id, weight, price, storagePeriod), flammability(flammability) {
        if (flammability < 0 || flammability > 4){
            throw std::invalid_argument("Flammability must be from 0 to 4");
        }
    }

    double calculateStorageFee() const override{
        double extra = (flammability < 3) ? 1.0 : ((flammability - 2) * 1.0) + 1.0;
        double res = Product::calculateStorageFee() * extra;
        return res;
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
        auto iter = std::find_if(products.begin(), products.end(), [product](const auto& prod){ return product->getId() == prod->getId(); });
        if (iter != products.end()) {
            throw std::runtime_error("Product with ID " + std::to_string(product->getId()) + " already exists.");
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

    void findProductsByCategory(const std::string& category) const {
        bool flag = false;

        for (const auto& product : products) {
            if ((category == "Perishable" && dynamic_cast<PerishableProduct*>(product.get())) ||
                (category == "Electronic" && dynamic_cast<ElectronicProduct*>(product.get())) ||
                (category == "BuildingMaterial" && dynamic_cast<BuildingMaterial*>(product.get()))) {
                flag = true;
                product.get()->displayInfo();
                std::cout << "--------------------\n";
            }
        }
        if (flag == false){
            std::cout << "Nothing was found for " << category << " category " << "\n"; 
        }
    }

    void displayInventory() const {
        std::cout << "\nBuilding Material products:" << "\n";
        findProductsByCategory("BuildingMaterial");
        std::cout << "\nElectronic products:" << "\n";
        findProductsByCategory("Electronic");
        std::cout << "\nPerishable products:" << "\n";
        findProductsByCategory("Perishable");
    }

    double calculateTotalStorageFee() const {
        double totalFee = 0;
        for (const auto &product: products) {
            totalFee += product->calculateStorageFee();
        }
        return totalFee;
    }

    std::vector<PerishableProduct> getExpiringProducts(const int days) const {
        if (days < 0){
            throw std::invalid_argument("Invalid days amount");
        }

        std::vector<PerishableProduct> res;
        for (const auto &product: products){
            auto prod = dynamic_cast<const PerishableProduct *>(product.get());
            if (prod && prod->getDate() < days) {
                res.push_back(*prod);
            }
        }

        return res;
    }

}; 




int main(){
    try{
        Warehouse ware;
        
        ware.addProduct(new PerishableProduct{"kolbasa", 12345, 0.4, 300, 12, 6});
        ware.addProduct(new PerishableProduct{"hleb", 819345, 0.4, 80, 10, 7});
        ware.addProduct(new BuildingMaterial{"Bricks", 78901, 500.0, 10000, 20, 4});
        ware.addProduct(new ElectronicProduct{"TV", 123456, 10.0, 2000, 24, 365, 150});

        // ware.displayAllProducts();
        std::cout << "Total fee: " << ware.calculateTotalStorageFee() << "\n";

        // std::cout << "\nSearching for Perishable products:\n";
        // ware.findProductsByCategory("Perishable");
        int thresholdDate = 10;
        auto expiringProducts = ware.getExpiringProducts(thresholdDate);

        // std::cout << "\nProducts expiring by " << thresholdDate << "days:\n";
        // for (const auto &product: expiringProducts) {
        //     product.displayInfo();
        //     std::cout << "--------------------\n";
        // }

        ware.displayInventory();
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
    }

    return 0;
}