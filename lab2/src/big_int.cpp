#include "big_int.h"
#include <string>

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
