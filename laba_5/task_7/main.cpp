#include <iostream>
#include <regex>

struct Date {
    int day;
    int month;
    int year;
};

int day_of_month(int month, int year){
    int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year % 4 == 0){
        days[1] = 29;
        if (year % 100 == 0 && year % 400 != 0) days[1] = 28;
    } 
    return days[month - 1];
}

Date parseDate(const std::string& dateStr) {
    // regexp of format "dd.mm.yyyy"
    std::regex dateRegex(R"((\d{2})\.(\d{2})\.(\d{4}))");
    std::smatch match;

    if (!std::regex_match(dateStr, match, dateRegex)) {
        throw std::invalid_argument("Неверный формат даты");
    }

    int day = std::stoi(match[1].str());
    int month = std::stoi(match[2].str());
    int year = std::stoi(match[3].str());

    if ((year > 2050 || year < 1980) || (month < 0 || month > 12)) {
        throw std::invalid_argument("Неверный формат даты");
    }
    if (day < 1 || day > day_of_month(month, year)) {
        throw std::invalid_argument("Неверный формат даты");
    }

    return Date{day, month, year};
}


class Product{
protected:
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

    virtual double calculateStorageFee() const { return weight * 100; }

    virtual void displayInfo() const {
        std::cout << 
            "Name: " << name << "\n" <<
            "ID: " << id << "\n" <<
            "Weight: " << weight << " kg" << "\n" << 
            "Price: $" << price << "\n" << 
            "Storage Period: " << expiration << " days" << "\n";
    }
};

class PerishableProduct: public Product{
private:
    Date expirationDate;
public:
    PerishableProduct(const std::string &name, unsigned int id, double weight, double price, int storagePeriod, std::string expirationDate): 
    Product(name, id, weight, price, storagePeriod){
        this->expirationDate = parseDate(expirationDate); 
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




int main(){
    try{
        
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
    }

    return 0;
}