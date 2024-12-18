#include <iostream>
#include <climits>

class binary_int {
private:
    int value;

    int sum(int num1, int num2) const {
        while (num2 != 0) {
            int carry = num1 & num2;
            num1 = num1 ^ num2;
            num2 = carry << 1;
        }
        return num1;
    }

    int negative(int n) const {
        return sum(~n, 1);
    }

    int sum_with_negative(int a, int b) const {
        return sum(a, negative(b));
    }

    int multiply(int a, int b) const {
        int result = 0;
        bool is_negative = (a < 0) ^ (b < 0);

        unsigned int ua = a < 0 ? negative(a) : a;
        unsigned int ub = b < 0 ? negative(b) : b;

        while (ub != 0) {
            if (ub & 1)
                result = sum(result, ua);
            ua <<= 1;
            ub >>= 1;
        }

        return is_negative ? negative(result) : result;
    }

    bool will_add_overflow(int a, int b) const {
        if (b > 0 && a > INT_MAX - b) return true;
        if (b < 0 && a < INT_MIN - b) return true;
        return false;
    }

    bool will_sub_overflow(int a, int b) const {
        if (b < 0 && a > INT_MAX + b) return true;
        if (b > 0 && a < INT_MIN + b) return true;
        return false;
    }

    bool will_mult_overflow(int a, int b) const {
        if (a == 0 || b == 0) return false;
        if (a > 0) {
            if (b > 0) {
                return a > INT_MAX / b;
            } else {
                return b < INT_MIN / a;
            }
        } else {
            if (b > 0) {
                return a < INT_MIN / b;
            } else {
                return b < INT_MAX / a;
            }
        }
    }

    bool will_shift_overflow(int a, int shift) const {
        if (shift < 0 || shift >= sizeof(int) * 8) return true;

        if (a == 0) return false;

        if (shift > 0) {

            if (a > 0 && (a > (INT_MAX >> shift))) return true;

            if (a < 0 && (a < (INT_MIN >> shift))) return true;
        }

        return false;
    }


public:
    explicit binary_int(int val = 0) {
        if (val < INT_MIN || val > INT_MAX) {
            throw std::overflow_error("overflow!");
        }
        value = val;
    }

    ~binary_int() = default;

    binary_int &operator-() {
        value = negative(value);
        return *this;
    }

    binary_int &operator++() {
        if (will_add_overflow(value, 1)) throw std::overflow_error("overflow!");
        value = sum(value, 1);
        return *this;
    }

    binary_int operator++(int) {
        if (will_add_overflow(value, 1)) throw std::overflow_error("overflow!");
        binary_int temp(*this);
        value = sum(value, 1);
        return temp;
    }

    binary_int &operator--() {
        if (will_sub_overflow(value, 1)) throw std::overflow_error("overflow!");
        value = sum_with_negative(value, 1);
        return *this;
    }

    binary_int operator--(int) {
        if (will_sub_overflow(value, 1)) throw std::overflow_error("overflow!");
        binary_int temp(*this);
        value = sum_with_negative(value, 1);
        return temp;
    }

    binary_int &operator+=(const binary_int &b_int) {
        if (will_add_overflow(value, b_int.get_value())) throw std::overflow_error("overflow!");
        value = sum(value, b_int.get_value());
        return *this;
    }

    binary_int operator+(const binary_int &other) const {
        binary_int result(*this);
        if (will_add_overflow(result.get_value(), other.get_value())) throw std::overflow_error("overflow!");
        return result += other;
    }

    binary_int &operator-=(const binary_int &b_int) {
        if (will_sub_overflow(value, b_int.get_value())) throw std::overflow_error("overflow!");
        value = sum_with_negative(value, b_int.get_value());
        return *this;
    }

    binary_int operator-(const binary_int &other) const {
        binary_int result(*this);
        if (will_sub_overflow(result.get_value(), other.get_value())) throw std::overflow_error("overflow!");
        return result -= other;
    }

    binary_int &operator*=(const binary_int &b_int) {
        if (will_mult_overflow(value, b_int.get_value())) throw std::overflow_error("overflow!");
        value = multiply(value, b_int.get_value());
        return *this;
    }

    binary_int operator*(const binary_int &other) const {
        binary_int result(*this);
        if (will_mult_overflow(result.get_value(), other.get_value())) throw std::overflow_error("overflow!");
        return result *= other;
    }

    binary_int &operator>>=(const binary_int &other) {
        if (will_shift_overflow(value, other.get_value())) throw std::overflow_error("overflow!");
        value >>= other.get_value();
        return *this;
    }

    binary_int &operator<<=(const binary_int &other) {
        if (will_shift_overflow(value, other.get_value())) throw std::overflow_error("overflow!");
        value <<= other.get_value();
        return *this;
    }

    binary_int operator>>(const binary_int &other) const {
        binary_int result(*this);
        if (will_shift_overflow(result.get_value(), other.get_value())) throw std::overflow_error("overflow!");
        return result >>= other;
    }

    binary_int operator<<(const binary_int &other) const {
        binary_int result(*this);
        if (will_shift_overflow(result.get_value(), other.get_value())) throw std::overflow_error("overflow!");
        return result <<= other;
    }

    std::pair<binary_int, binary_int> split() const {
        const int total_bits = sizeof(int) * 8;
        const int half_bits = total_bits / 2;

        int lower_mask = (1 << half_bits) - 1;
        int higher_mask = lower_mask << half_bits;

        binary_int higher((value & higher_mask) >> half_bits);
        binary_int lower(value & lower_mask);

        return std::make_pair(higher, lower);
    }



    int get_value() const {
        return value;
    }
};

std::ostream &operator <<(std::ostream &os, binary_int b){
    return os << b.get_value();
}


int main() {
    try {
        binary_int c(4);
        c += binary_int(2);
        std::cout << "result: " << c << std::endl;
        binary_int num1(65537);  // 0x00010001 - one bit in both higher and lower half
        auto [high1, low1] = num1.split();
        std::cout << "Test 1 (65537):" << std::endl;
        std::cout << "Higher half: " << high1 << " (expected 1)" << std::endl;
        std::cout << "Lower half: " << low1 << " (expected 1)" << std::endl;

        // Test 2: Number only in lower half
        binary_int num2(255);  // 0x000000FF - all bits in lower part
        auto [high2, low2] = num2.split();
        std::cout << "\nTest 2 (255):" << std::endl;
        std::cout << "Higher half: " << high2 << " (expected 0)" << std::endl;
        std::cout << "Lower half: " << low2 << " (expected 255)" << std::endl;

        // Test 3: Number only in higher half
        binary_int num3(16777216);  // 0x01000000 - one bit in higher part
        auto [high3, low3] = num3.split();
        std::cout << "\nTest 3 (16777216):" << std::endl;
        std::cout << "Higher half: " << high3 << " (expected 256)" << std::endl;
        std::cout << "Lower half: " << low3 << " (expected 0)" << std::endl;

        // Test 4: Maximum 16-bit number
        binary_int num4(65535);  // 0x0000FFFF - all bits in lower part
        auto [high4, low4] = num4.split();
        std::cout << "\nTest 4 (65535):" << std::endl;
        std::cout << "Higher half: " << high4 << " (expected 0)" << std::endl;
        std::cout << "Lower half: " << low4 << " (expected 65535)" << std::endl;

        // Test 5: Negative number
        binary_int num5(-65537);
        auto [high5, low5] = num5.split();
        std::cout << "\nTest 5 (-65537):" << std::endl;
        std::cout << "Higher half: " << high5 << std::endl;
        std::cout << "Lower half: " << low5 << std::endl;
        binary_int v(66);
        v *= (INT_MAX);
        std::cout << v << std::endl;
    } catch (const std::overflow_error &e) {
        std::cerr << "overflow!" << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
    }
    return 0;
}