#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <regex>


bool isValidDate(const std::string &date) {

    std::regex dateRegex(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}$)");
    if (!std::regex_match(date, dateRegex)) {
        return false;
    }


    int year, month, day, hour, minute, second;
    if (std::sscanf(date.c_str(), "%4d-%2d-%2d %2d:%2d:%2d",
                    &year, &month, &day, &hour, &minute, &second) != 6) {
        return false;
    }

    if (month < 1 || month > 12 || day < 1 || day > 31 ||
        hour < 0 || hour > 23 || minute < 0 || minute > 59 ||
        second < 0 || second > 59) {
        return false;
    }

    const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    int maxDay = (month == 2 && isLeapYear) ? 29 : daysInMonth[month - 1];

    if (day > maxDay) {
        return false;
    }

    return true;
}


class Product {
protected:
    std::string name;
    unsigned int id;
    double weight;
    double price;
    int storagePeriod;

public:
    Product(const std::string &name, unsigned int id, double weight, double price, int storagePeriod)
            : name(name), id(id), weight(weight), price(price), storagePeriod(storagePeriod) {
        if (weight < 0 || price < 0 || storagePeriod < 0) {
            throw std::invalid_argument("Weight, price, and storage period must be non-negative.");
        }
    }

    virtual ~Product() = default;

    virtual unsigned int getID() const { return id; }

    virtual void displayInfo() const {
        std::cout << "Name: " << name << "\nID: " << id
                  << "\nWeight: " << weight << " kg\nPrice: $" << price
                  << "\nStorage Period: " << storagePeriod << " days\n";
    }

    virtual double calculateStorageFee() const { return weight * 100; }

    unsigned int getId() const { return id; }
};

class PerishableProduct : public Product {
private:
    std::string expirationDate;

public:
    PerishableProduct(const std::string &name, unsigned int id, double weight, double price, int storagePeriod,
                      const std::string &expirationDate)
            : Product(name, id, weight, price, storagePeriod), expirationDate(expirationDate) {
        if (!isValidDate(expirationDate)) {
            throw std::invalid_argument("Invalid expiration date format. Use 'YYYY-MM-DD HH:MM:SS'.");
        }
    }

    void displayInfo() const override {
        Product::displayInfo();
        std::cout << "Expiration Date: " << expirationDate << "\n";
    }

    double calculateStorageFee() const override { return Product::calculateStorageFee() * 1.5; }

    const std::string &getExpirationDate() const { return expirationDate; }

    bool isExpiringSoon(const std::string &thresholdDate) const {
        if (!isValidDate(thresholdDate)) {
            throw std::invalid_argument("Invalid threshold date format. Use 'YYYY-MM-DD HH:MM:SS'.");
        }
        return expirationDate <= thresholdDate;
    }
};

class ElectronicProduct : public Product {
private:
    std::string warrantyPeriod;
    double powerRating;

public:
    ElectronicProduct(const std::string &name, unsigned int id, double weight, double price, int storagePeriod,
                      const std::string &warrantyPeriod, double powerRating)
            : Product(name, id, weight, price, storagePeriod),
              warrantyPeriod(warrantyPeriod), powerRating(powerRating) {
        if (powerRating < 0) {
            throw std::invalid_argument("Power rating must be non-negative.");
        }
    }

    void displayInfo() const override {
        Product::displayInfo();
        std::cout << "Warranty Period: " << warrantyPeriod
                  << "\nPower Rating: " << powerRating << " W\n";
    }
};

class BuildingMaterial : public Product {
private:
    bool flammability;

public:
    BuildingMaterial(const std::string &name, unsigned int id, double weight, double price, int storagePeriod,
                     bool flammability)
            : Product(name, id, weight, price, storagePeriod), flammability(flammability) {}

    void displayInfo() const override {
        Product::displayInfo();
        std::cout << "Flammability: " << (flammability ? "Yes" : "No") << "\n";
    }
};

class WareHouse {
private:
    std::vector<std::unique_ptr<Product>> products;

public:
    void addProduct(std::unique_ptr<Product> product) {
        if (!product) {
            throw std::invalid_argument("Cannot add null product to warehouse.");
        }
        products.push_back(std::move(product));
    }

    void removeProduct(unsigned int id) {
        auto it = std::remove_if(products.begin(), products.end(),
                                 [id](const auto &p) { return p->getId() == id; });
        if (it == products.end()) {
            throw std::runtime_error("Product with ID " + std::to_string(id) + " not found.");
        }
        products.erase(it, products.end());
    }

    void displayAllProducts() const {
        for (const auto &product: products) {
            product->displayInfo();
            std::cout << "--------------------\n";
        }
    }

    double calculateTotalStorageFee() const {
        double totalFee = 0;
        for (const auto &product: products) {
            totalFee += product->calculateStorageFee();
        }
        return totalFee;
    }

    std::vector<std::unique_ptr<PerishableProduct>> getExpiringProducts(const std::string &thresholdDate) const {
        if (!isValidDate(thresholdDate)) {
            throw std::invalid_argument("Invalid threshold date format. Use 'YYYY-MM-DD HH:MM:SS'.");
        }

        std::vector<std::unique_ptr<PerishableProduct>> expiringProducts;
        for (const auto &product: products) {
            auto perishableProduct = dynamic_cast<const PerishableProduct *>(product.get());
            if (perishableProduct && perishableProduct->isExpiringSoon(thresholdDate)) {
                expiringProducts.push_back(std::make_unique<PerishableProduct>(*perishableProduct));
            }
        }
        return expiringProducts;
    }
};

int main() {
    try {
        WareHouse warehouse;

        warehouse.addProduct(std::make_unique<PerishableProduct>("Milk", 1, 1.2, 1.5, 10, "2004-12-12 12:12:12"));
        warehouse.addProduct(std::make_unique<ElectronicProduct>("Laptop", 2, 2.5, 1200, 365, "2 years", 65));
        warehouse.addProduct(std::make_unique<BuildingMaterial>("Bricks", 3, 50, 500, 180, false));

        std::cout << "All products:\n";
        warehouse.displayAllProducts();

        std::cout << "Total storage fee: $" << warehouse.calculateTotalStorageFee() << "\n";

        std::string thresholdDate = "2024-12-31 23:59:59";
        auto expiringProducts = warehouse.getExpiringProducts(thresholdDate);

        std::cout << "\nProducts expiring by " << thresholdDate << ":\n";
        for (const auto &product: expiringProducts) {
            product->displayInfo();
            std::cout << "--------------------\n";
        }

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}
