#include <iostream>
#include <climits>

using namespace std;

class binary_int {
private:
    int value;

    static int sum(int num1, int num2) {
        while (num2 != 0) {
            int carry = num1 & num2;
            num1 = num1 ^ num2;
            num2 = carry << 1;
        }
        return num1;
    }

    static int negative(int n) {
        return sum(~n, 1);
    }

    static int sum_with_negative(int a, int b) {
        return sum(a, negative(b));
    }

    static int multiply(int a, int b) {
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

    static bool will_add_overflow(int a, int b) {
        if (b > 0 && a > INT_MAX - b) return true;
        if (b < 0 && a < INT_MIN - b) return true;
        return false;
    }

    static bool will_sub_overflow(int a, int b) {
        if (b < 0 && a > INT_MAX + b) return true;
        if (b > 0 && a < INT_MIN + b) return true;
        return false;
    }

    static bool will_mult_overflow(int a, int b) {
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

    static bool will_shift_overflow(int a, int shift) {
        if (shift < 0 || shift >= sizeof(int) * 8) return true;

        if (a == 0) return false;

        if (shift > 0) {

            if (a > 0 && (a > (INT_MAX >> shift))) return true;

            if (a < 0 && (a < (INT_MIN >> shift))) return true;
        }

        return false;
    }


public:
    binary_int(int val = 0) : value(val) {}

    ~binary_int() = default;

    binary_int &operator-() {
        value = negative(value);
        return *this;
    }

    binary_int &operator++() {
        if (will_add_overflow(value, 1)) throw overflow_error("overflow!");
        value = sum(value, 1);
        return *this;
    }

    binary_int operator++(int) {
        if (will_add_overflow(value, 1)) throw overflow_error("overflow!");
        binary_int temp(*this);
        value = sum(value, 1);
        return temp;
    }

    binary_int &operator--() {
        if (will_sub_overflow(value, 1)) throw overflow_error("overflow!");
        value = sum_with_negative(value, 1);
        return *this;
    }

    binary_int operator--(int) {
        if (will_sub_overflow(value, 1)) throw overflow_error("overflow!");
        binary_int temp(*this);
        value = sum_with_negative(value, 1);
        return temp;
    }

    binary_int &operator+=(const binary_int &b_int) {
        if (will_add_overflow(value, b_int.value)) throw overflow_error("overflow!");
        value = sum(value, b_int.value);
        return *this;
    }

    binary_int operator+(const binary_int &other) const {
        if (will_add_overflow(value, other.value)) throw overflow_error("overflow!");
        return binary_int(sum(value, other.value));
    }

    binary_int &operator-=(const binary_int &b_int) {
        if (will_sub_overflow(value, b_int.value)) throw overflow_error("overflow!");
        value = sum_with_negative(value, b_int.value);
        return *this;
    }

    binary_int operator-(const binary_int &other) const {
        if (will_sub_overflow(value, other.value)) throw overflow_error("overflow!");
        return binary_int(sum_with_negative(value, other.value));
    }

    binary_int &operator*=(const binary_int &b_int) {
        if (will_mult_overflow(value, b_int.value)) throw overflow_error("overflow!");
        value = multiply(value, b_int.value);
        return *this;
    }

    binary_int operator*(const binary_int &other) const {
        if (will_mult_overflow(value, other.value)) throw overflow_error("overflow!");
        return binary_int(multiply(value, other.value));
    }

    binary_int &operator>>=(int shift) {
        if (will_shift_overflow(value, shift)) throw overflow_error("overflow!");
        value >>= shift;
        return *this;
    }

    binary_int &operator<<=(int shift) {
        if (will_shift_overflow(value, shift)) throw overflow_error("overflow!");
        value <<= shift;
        return *this;
    }

    binary_int operator>>(const binary_int &other) const {
        if (will_shift_overflow(value, other.value)) throw overflow_error("overflow!");
        return binary_int(value >> other.value);
    }

    binary_int operator<<(const binary_int &other) const {
        if (will_shift_overflow(value, other.value)) throw overflow_error("overflow!");
        return binary_int(value << other.value);
    }

    pair<binary_int, binary_int> split() const {
        const int total_bits = sizeof(int) * 8;
        const int half_bits = total_bits / 2;

        int lower_mask = (1 << half_bits) - 1;
        int higher_mask = lower_mask << half_bits;

        binary_int higher((value & higher_mask) >> half_bits);
        binary_int lower(value & lower_mask);

        return make_pair(higher, lower);
    }

    friend ostream &operator<<(ostream &os, const binary_int &b) {
        return os << b.value;
    }

};


int main() {
    try {
        binary_int c(4);
        c <<= 22;
        cout << "result: " << c << endl;
        binary_int num1(65537);  // 0x00010001 - one bit in both higher and lower half
        auto [high1, low1] = num1.split();
        cout << "Test 1 (65537):" << endl;
        cout << "Higher half: " << high1 << " (expected 1)" << endl;
        cout << "Lower half: " << low1 << " (expected 1)" << endl;

        // Test 2: Number only in lower half
        binary_int num2(255);  // 0x000000FF - all bits in lower part
        auto [high2, low2] = num2.split();
        cout << "\nTest 2 (255):" << endl;
        cout << "Higher half: " << high2 << " (expected 0)" << endl;
        cout << "Lower half: " << low2 << " (expected 255)" << endl;

        // Test 3: Number only in higher half
        binary_int num3(16777216);  // 0x01000000 - one bit in higher part
        auto [high3, low3] = num3.split();
        cout << "\nTest 3 (16777216):" << endl;
        cout << "Higher half: " << high3 << " (expected 256)" << endl;
        cout << "Lower half: " << low3 << " (expected 0)" << endl;

        // Test 4: Maximum 16-bit number
        binary_int num4(65535);  // 0x0000FFFF - all bits in lower part
        auto [high4, low4] = num4.split();
        cout << "\nTest 4 (65535):" << endl;
        cout << "Higher half: " << high4 << " (expected 0)" << endl;
        cout << "Lower half: " << low4 << " (expected 65535)" << endl;

        // Test 5: Negative number
        binary_int num5(-65537);
        auto [high5, low5] = num5.split();
        cout << "\nTest 5 (-65537):" << endl;
        cout << "Higher half: " << high5 << endl;
        cout << "Lower half: " << low5 << endl;
        binary_int v(66);
        v *= (INT_MAX);
        cout << v << endl;
    } catch (const overflow_error &e) {
        cerr << "overflow!" << endl;
    }

    return 0;
}