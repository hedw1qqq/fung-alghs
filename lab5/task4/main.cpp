#include <iostream>
#include <cmath>
#include <stdexcept>

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
        if (is_overflow(real + other.real) || is_overflow(imagine + other.imagine)) {
            throw overflow_error("Overflow");
        }
        return Complex(real + other.real, imagine + other.imagine);
    }

    Complex operator-(const Complex &other) const {
        if (is_overflow(real - other.real) || is_overflow(imagine - other.imagine)) {
            throw overflow_error("Overflow");
        }
        return Complex(real - other.real, imagine - other.imagine);
    }

    Complex operator*(const Complex &other) const {
        double real_part1 = real * other.real;
        double real_part2 = imagine * other.imagine;
        double imagine_part1 = real * other.imagine;
        double imagine_part2 = imagine * other.real;

        if (is_overflow(real_part1 - real_part2) || is_overflow(imagine_part1 + imagine_part2)) {
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
        if (is_overflow(new_real) || is_overflow(new_imagine)) {
            throw overflow_error("Overflow ");
        }
        return Complex(new_real, new_imagine);
    }

    Complex &operator+=(const Complex &other) {
        if (is_overflow(real + other.real) || is_overflow(imagine + other.imagine)) {
            throw overflow_error("Overflow");
        }
        real += other.real;
        imagine += other.imagine;
        return *this;
    }

    Complex &operator-=(const Complex &other) {
        if (is_overflow(real - other.real) || is_overflow(imagine - other.imagine)) {
            throw overflow_error("Overflow ");
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

        if (is_overflow(real_part1 - real_part2) || is_overflow(imagine_part1 + imagine_part2)) {
            throw overflow_error("Overflow during multiplication");
        }

        real = real_part1 - real_part2;
        imagine = imagine_part1 + imagine_part2;
    }

    Complex &operator/=(const Complex &other) {
        double denominator = other.real * other.real + other.imagine * other.imagine;
        if (denominator == 0) {
            throw invalid_argument("Division by zero");
        }
        double new_real = (real * other.real + imagine * other.imagine) / denominator;
        double new_imagine = (imagine * other.real - real * other.imagine) / denominator;
        if (is_overflow(new_real) || is_overflow(new_imagine)) {
            throw overflow_error("Overflow");
        }
        real = new_real;
        imagine = new_imagine;
        return *this;
    }

    [[nodiscard]] double get_real() const { return real; }

    [[nodiscard]] double get_imagine() const { return imagine; }

    [[nodiscard]] double module() const { return sqrt(real * real + imagine * imagine); }

    [[nodiscard]] double argument() const { return atan2(imagine, real); }

private:

    [[nodiscard]] bool is_overflow(double value) const {
        return value > numeric_limits<double>::max() || value < -numeric_limits<double>::max();
    }
};


std::ostream &operator<<(std::ostream &os, const Complex &complex) {
    return os << "real = " << complex.get_real() << ", imagine = " << complex.get_imagine();
}

int main() {
    try {
        Complex a(1e308, 1e308);
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
