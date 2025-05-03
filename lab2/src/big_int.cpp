#include "big_int.h"

BigInt::BigInt() {
    digits.push_back(0);
    isNegative = false;
}

BigInt::BigInt(long long value) {
    isNegative = false;
    if (value < 0) {
        isNegative = true;
        value = -value;
    }
    while (value != 0) {
        digits.push_back(value % BASE);
        value /= BASE;
    }
}

BigInt::BigInt(const std::string &str) {
    std::string temp = str;
    if (temp.length() == 0) {
        BigInt();
    } else {
        isNegative = false;
        if (temp[0] == '-') {
            isNegative = true;
            temp = temp.substr(1);
        }
        if (std::all_of(temp.begin(), temp.end(), ::isdigit)) {}
        for (long long i = temp.length(); i > 0; i -= 9) {
            if (i < 9) {
                digits.push_back(atoi(temp.substr(0, i).c_str()));
            } else {
                digits.push_back(atoi(temp.substr(i - 9, 9).c_str()));
            }
        }
    }
}

BigInt::BigInt(const BigInt &other) {
    isNegative = other.isNegative;
    digits = std::vector<unsigned long long>(other.digits);
}

BigInt::BigInt(BigInt &&other) noexcept : digits(std::move(other.digits)) {
    isNegative = other.isNegative;
}

BigInt::~BigInt() {
    digits.clear();
}

std::ostream &operator<<(std::ostream &os, const BigInt &num) {
    if (num.digits.empty() || num.digits.back() == 0) {
        os << "0";
    } else {
        if (num.isNegative) {
            os << "-";
        }
        for (long long i = num.digits.size() - 1; i >= 0; i--) {
            os << num.digits[i];
        }
    }
    return os;
}

BigInt &BigInt::operator=(const BigInt &other) {
    if (this != &other) {
        isNegative = other.isNegative;
        digits = std::vector<unsigned long long>(other.digits);
    }
    return *this;
}

BigInt &BigInt::operator=(BigInt &&other) noexcept {
    if (this != &other) {
        isNegative = other.isNegative;
        digits = std::move(other.digits);
    }
    return *this;
}

BigInt BigInt::operator+(const BigInt &other) const {
    std::size_t thisSize = digits.size();
    std::size_t otherSize = other.digits.size();

    BigInt result = BigInt();
    long long add = 0;
    long long sign = isNegative ? (-1) : 1;
    if (thisSize < otherSize) {
        for (std::size_t i = 0; i < thisSize; i++) {
            long long temp = digits[i] * sign + other.digits[i] * sign + add * sign;
            if (temp >= BASE) {
                temp -= BASE;
                add = BASE;
            } else {
                add = 0;
            }
            result.digits.push_back(temp);
        }
        for (std::size_t i = thisSize; i < otherSize; i++) {
            long long temp = digits[i] * sign + add * sign;
            if (temp >= BASE) {
                temp -= BASE;
                add = BASE;
            } else {
                add = 0;
            }
            result.digits.push_back(temp);
        }
    }
    return result;
}
