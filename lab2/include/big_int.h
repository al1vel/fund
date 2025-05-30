#ifndef BIG_INT_H
#define BIG_INT_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <complex>
#include <cstdint>

#define BASE 1000000000
#define SMALL_BASE 10000

std::size_t num_length(uint64_t num);

class BigInt {
private:
    std::vector<uint64_t> digits;
    bool isNegative;

public:
    BigInt();
    explicit BigInt(long long value);
    explicit BigInt(const std::string &str);
    BigInt(const BigInt &other);
    BigInt(BigInt &&other) noexcept;
    ~BigInt();

    BigInt abs() const;
    void remove_leading_zeros();
    bool is_zero() const;
    BigInt operator%(const BigInt &other) const;
    void fft(std::vector<std::complex<long double>>& a, bool invert);
    std::string to_string() const;
    std::string small_number_to_string(std::vector<uint64_t>& num);
    std::vector<uint64_t> small_base_number(std::string& str) const;

    BigInt &operator=(const BigInt &other);
    BigInt &operator=(BigInt &&other) noexcept;
    BigInt operator+(const BigInt &other) const;
    BigInt operator-(const BigInt &other) const;
    BigInt operator*(const BigInt &other) const;
    BigInt operator/(const BigInt &other) const;
    BigInt& operator+=(const BigInt &other);
    BigInt& operator-=(const BigInt &other);
    BigInt& operator*=(const BigInt &other);
    BigInt& operator/=(const BigInt &other);
    BigInt& operator++();
    BigInt& operator--();

    bool operator==(const BigInt &other) const;
    bool operator!=(const BigInt &other) const;
    bool operator<(const BigInt &other) const;
    bool operator>(const BigInt &other) const;
    bool operator<=(const BigInt &other) const;
    bool operator>=(const BigInt &other) const;

    [[nodiscard]] BigInt mod_exp(const BigInt &exp, const BigInt &mod) const;
    [[nodiscard]] BigInt fft_multiply(const BigInt &a);
    [[nodiscard]] BigInt fft_multiply2(const BigInt &a);
    [[nodiscard]] BigInt karatsuba_multiply(const BigInt &a) const;
    [[nodiscard]] BigInt newton_divide(const BigInt &a) const;

    friend std::istream &operator>>(std::istream &is, BigInt &num);
    friend std::ostream &operator<<(std::ostream &os, const BigInt &num);
};

#endif //BIG_INT_H