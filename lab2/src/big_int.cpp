#include "big_int.h"

std::size_t num_length(uint64_t num) {
    std::size_t length = 0;
    while (num) {
        num /= 10;
        length++;
    }
    return length;
}


BigInt::BigInt() {
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
        if (!std::all_of(temp.begin(), temp.end(), ::isdigit)) {
            throw std::invalid_argument("invalid number");
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
    digits = std::vector<uint64_t>(other.digits);
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
            if (num.digits[i] == 0) {
                os << "000000000";
            } else {
                if (i != static_cast<long long>(num.digits.size()) - 1) {
                    std::size_t len = num_length(num.digits[i]);
                    if (len < 9) {
                        std::string s(9 - len, '0');
                        os << s;
                    }
                }
                os << num.digits[i];
            }
        }
    }
    return os;
}

BigInt &BigInt::operator=(const BigInt &other) {
    if (this != &other) {
        isNegative = other.isNegative;
        digits = std::vector<uint64_t>(other.digits);
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

BigInt BigInt::abs() const {
    BigInt temp = BigInt(*this);
    temp.isNegative = false;
    return temp;
}


bool BigInt::operator==(const BigInt &other) const {
    if (isNegative != other.isNegative) {
        return false;
    }
    if (digits.size() != other.digits.size()) {
        return false;
    }
    for (std::size_t i = 0; i < digits.size(); ++i) {
        if (digits[i] != other.digits[i]) {
            return false;
        }
    }
    return true;
}

bool BigInt::operator!=(const BigInt &other) const {
    return !(*this == other);
}

bool BigInt::operator<(const BigInt &other) const {
    if (isNegative && !other.isNegative) {
        return true;
    }
    if (!isNegative && other.isNegative) {
        return false;
    }
    if (digits.size() > other.digits.size()) {
        return false;
    }
    if (digits.size() < other.digits.size()) {
        return true;
    }
    bool findLess = false;
    for (std::size_t i = 0; i < digits.size(); ++i) {
        if (!isNegative) {
            if (digits[i] > other.digits[i]) {
                return false;
            }
            if (digits[i] < other.digits[i]) {
                findLess = true;
            }
        } else {
            if (digits[i] < other.digits[i]) {
                return false;
            }
            if (digits[i] > other.digits[i]) {
                findLess = true;
            }
        }
    }
    return findLess;
}

bool BigInt::operator>(const BigInt &other) const {
    if (*this == other) {
        return false;
    }
    if (*this < other) {
        return false;
    }
    return true;
}

bool BigInt::operator<=(const BigInt &other) const {
    if (*this > other) {
        return false;
    }
    return true;
}

bool BigInt::operator>=(const BigInt &other) const {
    if (*this < other) {
        return false;
    }
    return true;
}

void BigInt::remove_leading_zeros() {
    while (digits.back() == 0 && digits.size() > 1) {
        digits.pop_back();
    }
}


BigInt BigInt::operator+(const BigInt &other) const {
    BigInt result = BigInt();

    if (!isNegative && !other.isNegative) {
        std::size_t n = std::max(digits.size(), other.digits.size());
        uint64_t add = 0;
        for (std::size_t i = 0; i < n; ++i) {
            uint64_t sum = add;
            if (i < digits.size()) {
                sum += digits[i];
            }
            if (i < other.digits.size()) {
                sum += other.digits[i];
            }
            result.digits.push_back(sum % BASE);
            add = sum / BASE;
        }

    } else if (isNegative && other.isNegative) {
        std::size_t n = std::max(digits.size(), other.digits.size());
        uint64_t add = 0;
        for (std::size_t i = 0; i < n; ++i) {
            uint64_t sum = add;
            if (i < digits.size()) {
                sum += digits[i];
            }
            if (i < other.digits.size()) {
                sum += other.digits[i];
            }
            result.digits.push_back(sum % BASE);
            add = sum / BASE;
        }
        result.isNegative = true;

    } else {
        BigInt absThis = this->abs();
        BigInt absOther = other.abs();
        BigInt left, right;

        if (absThis == absOther) {
            result.digits.push_back(0);
            return result;
        }
        if (absThis > absOther) {
            left = *this;
            right = other;
        } else {
            left = other;
            right = *this;
        }
        if (left.isNegative && !right.isNegative) {
            result.isNegative = true;
        }
        int64_t add = 0;
        for (std::size_t i = 0; i < left.digits.size(); ++i) {
            int64_t sum = add;
            sum += left.digits[i];
            if (i < right.digits.size()) {
                sum -= right.digits[i];
            }
            result.digits.push_back((sum + BASE) % BASE);
            if (sum < 0) {
                add = -1;
            } else {
                add = sum / BASE;
            }
        }
    }
    result.remove_leading_zeros();
    return result;
}


