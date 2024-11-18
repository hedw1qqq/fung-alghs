#include <iostream>
#include <cmath>
#include <stdexcept>
#include <limits>

using namespace std;

class Complex {
private:
    double real;
    double imagine;

public:
    ~Complex() = default;

    Complex(double real = 0, double imagine = 0) : real(real), imagine(imagine) {}

    Complex &operator=(const Complex &other) noexcept {
        if (this != &other) {
            real = other.real;
            imagine = other.imagine;
        }
        return *this;
    }

    Complex operator+(const Complex &other) const {
        if (will_add_overflow(real, other.real) || will_add_overflow(imagine, other.imagine)) {
            throw overflow_error("Overflow during addition");
        }
        return Complex(real + other.real, imagine + other.imagine);
    }

    Complex operator-(const Complex &other) const {
        if (will_sub_overflow(real, other.real) || will_sub_overflow(imagine, other.imagine)) {
            throw overflow_error("Overflow during subtraction");
        }
        return Complex(real - other.real, imagine - other.imagine);
    }

    Complex operator*(const Complex &other) const {
        double real_part1 = real * other.real;
        double real_part2 = imagine * other.imagine;
        double imagine_part1 = real * other.imagine;
        double imagine_part2 = imagine * other.real;

        if (will_sub_overflow(real_part1, real_part2) || will_add_overflow(imagine_part1, imagine_part2)) {
            throw overflow_error("Overflow during multiplication");
        }

        return Complex(real_part1 - real_part2, imagine_part1 + imagine_part2);
    }

    Complex operator/(const Complex &other) const {
        double denominator = other.real * other.real + other.imagine * other.imagine;
        if (denominator == 0) {
            throw invalid_argument("Division by zero");
        }
        double new_real = (real * other.real + imagine * other.imagine) / denominator;
        double new_imagine = (imagine * other.real - real * other.imagine) / denominator;

        return Complex(new_real, new_imagine);
    }

    Complex &operator+=(const Complex &other) {
        if (will_add_overflow(real, other.real) || will_add_overflow(imagine, other.imagine)) {
            throw overflow_error("Overflow during addition");
        }
        real += other.real;
        imagine += other.imagine;
        return *this;
    }

    Complex &operator-=(const Complex &other) {
        if (will_sub_overflow(real, other.real) || will_sub_overflow(imagine, other.imagine)) {
            throw overflow_error("Overflow during subtraction");
        }
        real -= other.real;
        imagine -= other.imagine;
        return *this;
    }

    Complex &operator*=(const Complex &other) {
        double real_part1 = real * other.real;
        double real_part2 = imagine * other.imagine;
        double imagine_part1 = real * other.imagine;
        double imagine_part2 = imagine * other.real;

        if (will_sub_overflow(real_part1, real_part2) || will_add_overflow(imagine_part1, imagine_part2)) {
            throw overflow_error("Overflow during multiplication");
        }

        real = real_part1 - real_part2;
        imagine = imagine_part1 + imagine_part2;
        return *this;
    }

    Complex &operator/=(const Complex &other) {
        double denominator = other.real * other.real + other.imagine * other.imagine;
        if (denominator == 0) {
            throw invalid_argument("Division by zero");
        }
        double new_real = (real * other.real + imagine * other.imagine) / denominator;
        double new_imagine = (imagine * other.real - real * other.imagine) / denominator;

        real = new_real;
        imagine = new_imagine;
        return *this;
    }

    double get_real() const { return real; }

    double get_imagine() const { return imagine; }

    double module() const { return sqrt(real * real + imagine * imagine); }

    double argument() const { return atan2(imagine, real); }

private:
    static bool will_add_overflow(double a, double b) {
        return (b > 0 && a > numeric_limits<double>::max() - b) ||
               (b < 0 && a < numeric_limits<double>::lowest() - b);
    }

    static bool will_sub_overflow(double a, double b) {
        return (b < 0 && a > numeric_limits<double>::max() + b) ||
               (b > 0 && a < numeric_limits<double>::lowest() + b);
    }

    static bool will_mult_overflow(double a, double b) {
        if (a == 0 || b == 0) return false;
        if (a > 0) {
            if (b > 0) {
                return a > numeric_limits<double>::max() / b;
            } else {
                return b < numeric_limits<double>::lowest() / a;
            }
        } else {
            if (b > 0) {
                return a < numeric_limits<double>::lowest() / b;
            } else {
                return b < numeric_limits<double>::max() / a;
            }
        }
    }
};

std::ostream &operator<<(std::ostream &os, const Complex &complex) {
    return os << "real = " << complex.get_real() << ", imagine = " << complex.get_imagine();
}

int main() {
    try {
        Complex a(4343, 0);
        Complex b(1e-308, 1e-308);
        Complex c;

        cout << "Number a: " << a << endl;
        cout << "Number b: " << b << endl;

        c = a + b;
        cout << "a + b: " << c << endl;

        c = a - b;
        cout << "a - b: " << c << endl;

        c = a * b;
        cout << "a * b: " << c << endl;

        c = a / Complex(0, 0);
        cout << "a / 0: " << c << endl;

    } catch (const invalid_argument &e) {
        cerr << "Error: " << e.what() << endl;
    } catch (const overflow_error &e) {
        cerr << "Overflow error: " << e.what() << endl;
    } catch (...) {
        cerr << "Unknown error occurred!" << endl;
    }

    return 0;
}
