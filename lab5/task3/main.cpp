#include <iostream>
#include <stdexcept>
#include <cstring>

using namespace std;

class LogicalValuesArray {
private:
    unsigned int value;

public:

    LogicalValuesArray(unsigned int initial_value = 0) {
        value = initial_value;
    }

    unsigned int get_value() const {
        return value;
    }

    LogicalValuesArray inversion() const {
        return LogicalValuesArray(~value);
    }

    LogicalValuesArray conjunction(const LogicalValuesArray &other) const {
        return LogicalValuesArray(value & other.value);
    }

    LogicalValuesArray disjunction(const LogicalValuesArray &other) const {
        return LogicalValuesArray(value | other.value);
    }

    LogicalValuesArray implication(const LogicalValuesArray &other) const {
        return LogicalValuesArray(~value | other.value); // !A | B
    }

    LogicalValuesArray coimplication(const LogicalValuesArray &other) const {
        return LogicalValuesArray(value & ~other.value); // A & !B
    }

    LogicalValuesArray xor_modulo2(const LogicalValuesArray &other) const {
        return LogicalValuesArray(value ^ other.value);
    }

    LogicalValuesArray equivalence(const LogicalValuesArray &other) const {
        return xor_modulo2(other).inversion(); // !(A ^ B)
    }

    LogicalValuesArray pierce_arrow(const LogicalValuesArray &other) const {
        return LogicalValuesArray(~(value | other.value)); // !(A | B)
    }

    LogicalValuesArray sheffer_stroke(const LogicalValuesArray &other) const {
        return LogicalValuesArray(~(value & other.value)); // !(A & B)
    }

    static bool equals(const LogicalValuesArray &a, const LogicalValuesArray &b) {
        return a.get_value() == b.get_value();
    }

    bool get_bit(int position) const {
        if (position < 0 || position >= sizeof(unsigned int) * 8) {
            throw out_of_range("Bit position must be between 0 and 31.");
        }
        return (value >> position) & 1;
    }

    void to_binary_string(char *binary_str, size_t size) const {
        if (binary_str == nullptr) {
            throw invalid_argument("Binary string buffer cannot be null.");
        }
        size_t binary_length = sizeof(unsigned int) * 8;
        if (size < binary_length + 1) {
            throw length_error("Buffer size too small");
        }

        for (size_t i = 0; i < binary_length; i++) {
            binary_str[i] = ((value >> (binary_length - 1 - i)) & 1) + '0';
        }
        binary_str[binary_length] = '\0';
    }
};




int main() {
    try {
        LogicalValuesArray a(5); // 5 в двоичной форме: 00000000000000000000000000000101
        LogicalValuesArray b(3); // 3 в двоичной форме: 00000000000000000000000000000011

        cout << "a: " << a.get_value() << ", b: " << b.get_value() << endl;

        cout << "Inversion of a: " << a.inversion().get_value() << endl;
        cout << "Conjunction (AND) a & b: " << a.conjunction(b).get_value() << endl;
        cout << "Disjunction (OR) a | b: " << a.disjunction(b).get_value() << endl;
        cout << "Implication a -> b: " << a.implication(b).get_value() << endl;
        cout << "Equivalence a == b: " << a.equivalence(b).get_value() << endl;
        cout << "XOR a ^ b: " << a.xor_modulo2(b).get_value() << endl;
        cout << "Pierce Arrow (NOR) a NOR b: " << a.pierce_arrow(b).get_value() << endl;
        cout << "Sheffer Stroke (NAND) a NAND b: " << a.sheffer_stroke(b).get_value() << endl;

        cout << "Equality check (a == b): " << (LogicalValuesArray::equals(a, b) ? "true" : "false") << endl;

        cout << "Bit 0 of a: " << a.get_bit(0) << endl;
        cout << "Bit 1 of a: " << a.get_bit(1) << endl;

        char binary_str[sizeof(unsigned int) * 8 + 1];
        a.to_binary_string(binary_str, sizeof(binary_str));
        cout << "Binary representation of a: " << binary_str << endl;
    }
    catch (const overflow_error &e) {
        cerr << "Overflow!" << endl;
    }
    catch (const invalid_argument &e) {
        cerr << "Invalid argument: " << e.what() << endl;
    }
    catch (const out_of_range &e) {
        cerr << "Out of range: " << e.what() << endl;
    }
    catch (const length_error &e) {
        cerr << "Length error: " << e.what() << endl;
    }
    catch (...) {
        cerr << "Unknown error occurred!" << endl;
    }
    return 0;
}
