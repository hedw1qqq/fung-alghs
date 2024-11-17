#include <iostream>
#include <bitset>
#include <cmath>
#include <stdexcept>
#include <cstring>

using namespace std;

class LogicalValuesArray {
private:
    unsigned int value;

public:
    LogicalValuesArray(unsigned int initial_value = 0) {
        if (initial_value > numeric_limits<unsigned int>::max()) {
            throw std::overflow_error("Value exceeds maximum for unsigned int.");
        }
        value = initial_value;
    }


    unsigned int get_value() const {
        return value;
    }

    unsigned int inversion() const {
        return ~value;
    }


    unsigned int conjunction(const LogicalValuesArray &other) const {
        return value & other.get_value();
    }

    unsigned int disjunction(const LogicalValuesArray &other) const {
        return value | other.get_value();
    }

    unsigned int implication(const LogicalValuesArray &other) const {
        return ~(value & ~other.get_value()); // !(A && !B)
    }

    unsigned int coimplication(const LogicalValuesArray &other) const {
        return ~(value ^ other.get_value()); // !(A ^ B)
    }


    unsigned int xor_modulo2(const LogicalValuesArray &other) const {
        return value ^ other.get_value();
    }

    unsigned int equivalence(const LogicalValuesArray &other) const {
        return !(value ^ other.get_value()); // !(A ^ B)
    }

    unsigned int pierce_arrow(const LogicalValuesArray &other) const {
        return ~(value & other.get_value()); // !(A && B)
    }

    unsigned int sheffer_stroke(const LogicalValuesArray &other) const {
        return ~(value | other.get_value()); // !(A || B)
    }

    static bool equals(const LogicalValuesArray &a, const LogicalValuesArray &b) {
        return a.get_value() == b.get_value();
    }

    bool get_bit(int position) const {
        if (position < 0 || position >= 32) {
            throw std::out_of_range("Bit position must be between 0 and 31.");
        }
        return (value >> position) & 1;
    }


    void to_binary_string(char *binary_str) const {
        if (binary_str == nullptr) {
            throw std::invalid_argument("Binary string buffer cannot be null.");
        }
        bitset<32> bits(value);
        strcpy(binary_str, bits.to_string().c_str());
    }

};

int main() {
    try {
        LogicalValuesArray a(5); // 5 в двоичной форме: 00000000000000000000000000000101
        LogicalValuesArray b(3); // 3 в двоичной форме: 00000000000000000000000000000011

        std::cout << "a: " << a.get_value() << ", b: " << b.get_value() << std::endl;

        std::cout << "Inversion of a: " << a.inversion() << std::endl;
        std::cout << "Conjunction (AND) a & b: " << a.conjunction(b) << std::endl;
        std::cout << "Disjunction (OR) a | b: " << a.disjunction(b) << std::endl;
        std::cout << "Implication a -> b: " << a.implication(b) << std::endl;
        std::cout << "Equivalence a == b: " << a.equivalence(b) << std::endl;
        std::cout << "XOR a ^ b: " << a.xor_modulo2(b) << std::endl;
        std::cout << "Pierce Arrow (NAND) a NAND b: " << a.pierce_arrow(b) << std::endl;
        std::cout << "Sheffer Stroke (NOR) a NOR b: " << a.sheffer_stroke(b) << std::endl;

        std::cout << "Equality check (a == b): " << (LogicalValuesArray::equals(a, b) ? "true" : "false") << std::endl;

        std::cout << "Bit 0 of a: " << a.get_bit(0) << std::endl;
        std::cout << "Bit 2 of a: " << a.get_bit(2) << std::endl;

        char binary_str[33]; // Для хранения двоичной строки
        a.to_binary_string(binary_str);
        std::cout << "Binary representation of a: " << binary_str << std::endl;
    }
    catch (const overflow_error &e) {
        cerr << "Overflow!" << endl;
    }
    catch (const invalid_argument &e) {
        cerr << "null pointer!" << endl;
    }
    catch (const out_of_range &e) {
        cerr << "Bit position must be between 0 and 31." << endl;
    }
    return 0;
}
