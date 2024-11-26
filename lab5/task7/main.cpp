#include <regex>
#include <stdexcept>
#include <ctime>
#include <string>
#include <iostream>

time_t parseDate(const std::string &date) {

    std::regex dateRegex(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}$)");
    if (!std::regex_match(date, dateRegex)) {
        throw std::invalid_argument("Invalid date format. Use 'YYYY-MM-DD HH:MM:SS'.");
    }

    std::tm timeInfo{};
    try {
        timeInfo.tm_year = std::stoi(date.substr(0, 4)) - 1900; // Год с 1900
        timeInfo.tm_mon = std::stoi(date.substr(5, 2)) - 1;     // Месяцы от 0 до 11
        timeInfo.tm_mday = std::stoi(date.substr(8, 2));
        timeInfo.tm_hour = std::stoi(date.substr(11, 2));
        timeInfo.tm_min = std::stoi(date.substr(14, 2));
        timeInfo.tm_sec = std::stoi(date.substr(17, 2));
    } catch (const std::exception &) {
        throw std::invalid_argument("Invalid date values in input string.");
    }

    time_t result = std::mktime(&timeInfo);
    if (result == -1) {
        throw std::invalid_argument("Failed to convert date to time_t.");
    }

    return result;
}


class Product {
private:
    std::string name;
    unsigned int id;
    double weight;
    double price;
    unsigned int storagePeriod;

    void validate() const {
        if (name.empty() || weight < 0 || price < 0) {
            throw std::invalid_argument("Weight, price, and storage period must be non-negative.");
        }
    }

public:
    explicit Product(const std::string &name, unsigned int id, double weight, double price, unsigned int storagePeriod)
            : name(name), id(id), weight(weight), price(price), storagePeriod(storagePeriod) {
        validate();
    }

    Product(const Product &other)
            : name(other.name), id(other.id), weight(other.weight), price(other.price),
              storagePeriod(other.storagePeriod) {
        validate();
    }

    Product &operator=(const Product &other) {
        if (this != &other) {
            name = other.name;
            id = other.id;
            weight = other.weight;
            price = other.price;
            storagePeriod = other.storagePeriod;
            validate();
        }
        return *this;
    }

    virtual ~Product() = default;

    virtual unsigned int getID() const { return id; }

    virtual void displayInfo() const {
        std::cout << "Name: " << name << "\nID: " << id
                  << "\nWeight: " << weight << " kg\nPrice: $" << price
                  << "\nStorage Period: " << storagePeriod << " days\n";
    }

    virtual double calculateStorageFee() const { return weight * 100; }

    virtual std::string getCategory() const { return "Product"; }

    virtual unsigned int getId() const { return id; }
};

class PerishableProduct : public Product {
private:
    time_t expirationDate;

public:
    explicit PerishableProduct(const std::string &name, unsigned int id, double weight, double price,
                               unsigned int storagePeriod,
                               const std::string &expirationDateStr)
            : Product(name, id, weight, price, storagePeriod), expirationDate(parseDate(expirationDateStr)) {}

    PerishableProduct(const PerishableProduct &other)
            : Product(other), expirationDate((other.expirationDate)) {}

    PerishableProduct &operator=(const PerishableProduct &other) {
        if (this != &other) {
            Product::operator=(other);
            expirationDate = other.expirationDate;
        }
        return *this;
    }

    void displayInfo() const override {
        Product::displayInfo();
        std::tm *timeInfo = std::localtime(&expirationDate);
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);
        std::cout << "Expiration Date: " << buffer << "\nCategory: " << getCategory() << std::endl;
    }

    double calculateStorageFee() const override {
        time_t now = time(nullptr);
        double daysUntilExpiration = difftime(expirationDate, now) / (24 * 60 * 60);

        if (daysUntilExpiration <= 0) {
            return Product::calculateStorageFee() * 2.0;
        } else if (daysUntilExpiration <= 7) {
            return Product::calculateStorageFee() * 1.5;
        } else if (daysUntilExpiration <= 30) {
            return Product::calculateStorageFee() * 1.2;
        } else {
            return Product::calculateStorageFee();
        }
    }

    bool isExpiringSoon(time_t thresholdDate) const {
        return expirationDate <= thresholdDate;
    }

    std::string getCategory() const override { return "PerishableProduct"; }
};

class ElectronicProduct : public Product {
private:
    time_t warrantyPeriod;
    double powerRating;

public:
    explicit ElectronicProduct(const std::string &name, unsigned int id, double weight, double price, int storagePeriod,
                               const std::string &warrantyPeriod, double powerRating)
            : Product(name, id, weight, price, storagePeriod),
              warrantyPeriod(parseDate(warrantyPeriod)), powerRating(powerRating) {
        if (powerRating < 0) {
            throw std::invalid_argument("Power rating must be non-negative.");
        }

    }

    std::string getCategory() const override { return "ElectronicProduct"; }

    ElectronicProduct(const ElectronicProduct &other)
            : Product(other), warrantyPeriod(other.warrantyPeriod), powerRating(other.powerRating) {
        if (powerRating < 0) {
            throw std::invalid_argument("Power rating must be non-negative.");
        }
    }

    ElectronicProduct &operator=(const ElectronicProduct &other) {
        if (this != &other) {
            Product::operator=(other);
            warrantyPeriod = other.warrantyPeriod;
            powerRating = other.powerRating;
        }
        return *this;
    }

    void displayInfo() const override {
        Product::displayInfo();
        std::tm *timeInfo = std::localtime(&warrantyPeriod);
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);
        std::cout << "Warranty Period: " << warrantyPeriod
                  << "\nPower Rating: " << powerRating << " W\nCategory: " << getCategory() << std::endl;;
    }
};

class BuildingMaterial : public Product {
private:
    bool flammability;

public:
    BuildingMaterial(const std::string &name, unsigned int id, double weight, double price, int storagePeriod,
                     bool flammability)
            : Product(name, id, weight, price, storagePeriod), flammability(flammability) {}

    BuildingMaterial(const BuildingMaterial &other)
            : Product(other), flammability(other.flammability) {}

    BuildingMaterial &operator=(const BuildingMaterial &other) {
        if (this != &other) {
            Product::operator=(other);
            flammability = other.flammability;
        }
        return *this;
    }

    double calculateStorageFee() const override {
        if (flammability) {
            return 5 * Product::calculateStorageFee();
        }
        return Product::calculateStorageFee();
    }

    std::string getCategory() const override { return "BuildingMaterial"; }

    void displayInfo() const override {
        Product::displayInfo();
        std::cout << "Flammability: " << (flammability ? "Yes" : "No") << "\nCategory: " << getCategory() << std::endl;
    }
};

class WareHouse {
private:
    std::vector<Product *> products;


public:
    WareHouse() = default;


    ~WareHouse() {
        for (auto product: products) {
            delete product;
        }
    }

    WareHouse &operator+=(Product *product) {
        auto it = std::find_if(products.begin(), products.end(),
                               [product](Product *p) { return p->getID() == product->getID(); });
        if (it != products.end()) {
            throw std::runtime_error("Product with this ID already exists in the warehouse.");
        }
        products.push_back(product);
        return *this;
    }

    WareHouse &operator-=(unsigned int id) {
        auto it = std::find_if(products.begin(), products.end(),
                               [id](Product *p) { return p->getID() == id; });
        if (it == products.end()) {
            throw std::runtime_error("Product with this ID does not exist in the warehouse.");
        }
        delete *it;
        products.erase(it);
        return *this;
    }

    Product *operator[](unsigned int id) const {
        auto it = std::find_if(products.begin(), products.end(),
                               [id](Product *p) { return p->getID() == id; });
        if (it == products.end()) {
            return nullptr;
        }
        return *it;
    }

    void displayAllProducts() {
        std::sort(products.begin(), products.end(),
                  [](const Product *left, const Product *right) {
                      return left->getCategory() < right->getCategory();
                  });
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

    std::vector<Product *> getExpiringProducts(const int days) const {
        std::vector<Product *> expiringProducts;
        time_t now = time(nullptr);
        time_t thresholdDate = now + days * 24 * 60 * 60;

        for (const auto &product: products) {
            auto *perishable = dynamic_cast<PerishableProduct *>(product);
            if (perishable && perishable->isExpiringSoon(thresholdDate)) {
                expiringProducts.push_back(perishable);
            }
        }
        return expiringProducts;
    }

    std::vector<Product *> getProducts() const {
        return products;
    }

    std::vector<Product *> getCategoryProducts(const std::string &category) const {
        std::vector<Product *> answer;
        for (auto *i: products) {
            if (i->getCategory() == category) {
                answer.push_back(i);
            }
        }
        return answer;
    }

};


std::ostream &operator<<(std::ostream &os, const Product &product) {
    product.displayInfo();
    return os;
}

std::ostream &operator<<(std::ostream &os, const WareHouse &wareHouse) {
    const auto &products = wareHouse.getProducts();

    if (products.empty()) {
        os << "Warehouse is empty.\n";
        return os;
    }

    for (auto const product: products) {
        if (product != nullptr) {
            os << *product;
            os << "----------------------------------------\n";
        }
    }
    return os;
}

int main() {
    try {
        WareHouse warehouse;

        // Adding products manually

        warehouse += new PerishableProduct("Cheese", 2, 2.0, 5.0, 15, "2024-12-05 10:00:00");
        warehouse += new ElectronicProduct("Laptop", 3, 3.0, 1200.0, 30, "2025-01-01 00:00:00", 65.0);
        warehouse += new PerishableProduct("Milk", 1, 1.5, 2.5, 10, "2024-12-01 10:00:00");
        warehouse += new ElectronicProduct("Phone", 4, 1.0, 800.0, 20, "2025-01-15 00:00:00", 15.0);
        warehouse += new BuildingMaterial("Cement", 5, 50.0, 300.0, 60, false);
        warehouse += new BuildingMaterial("Paint", 6, 10.0, 50.0, 20, true);
        Product *product = warehouse[2];
        if (product != nullptr) {
            product->displayInfo();
        } else {
            std::cout << "Product with ID 2 not found.\n";
        }

        // Display all products
        std::cout << "=== All Products ===\n";
        warehouse.displayAllProducts();

        warehouse -= 5;
        warehouse -= 3;
        // Display remaining products
        std::cout << "\n=== Remaining Products ===\n";
        warehouse.displayAllProducts();

        // Calculate total storage fee
        double totalFee = warehouse.calculateTotalStorageFee();
        std::cout << "\nTotal storage fee: $" << totalFee << "\n";

        // Find products expiring in the next 7 days
        int days = 7;
        auto expiringProducts = warehouse.getExpiringProducts(days);

        std::cout << "\n=== Products Expiring in the Next " << days << " Days ===\n";
        for (const auto &i: expiringProducts) {
            i->displayInfo();
            std::cout << "--------------------\n";
        }

        std::cout << "Perishable products" << std::endl;
        auto ans = warehouse.getCategoryProducts("PerishableProduct");
        for (const auto &i: ans) {
            product->displayInfo();
            std::cout << "--------------------\n";
        }

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}

