#include "big_int.h"

#include <complex>

std::size_t num_length(uint64_t num) {
    std::size_t length = 0;
    while (num) {
        num /= 10;
        length++;
    }
    return length;
}

bool BigInt::is_zero() const {
    if (this->digits.empty()) {
        return true;
    }
    if (this->digits.size() == 1 && this->digits[0] == 0) {
        return true;
    }
    return false;
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
        this->remove_leading_zeros();
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
    if (num.digits.empty() || (num.digits.back() == 0 && num.digits.size() == 1)) {
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
    int n = digits.size();
    for (int i = n - 1; i >= 0; --i) {
        if (!isNegative) {
            if (digits[i] > other.digits[i]) {
                if (!findLess) {
                    return false;
                }
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

    if (!(isNegative ^ other.isNegative)) {
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
        if (add > 0) {
            result.digits.push_back(add % BASE);
        }
        result.isNegative = other.isNegative;

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

BigInt BigInt::operator-(const BigInt &other) const {
    BigInt otherTmp = other;
    otherTmp.isNegative = !otherTmp.isNegative;
    BigInt result = *this + otherTmp;
    return result;
}

BigInt BigInt::operator*(const BigInt &other) const {
    BigInt result;
    result.digits.resize(digits.size() + other.digits.size(), 0);

    for (std::size_t i = 0; i < digits.size(); ++i) {
        uint64_t add = 0;
        for (std::size_t j = 0; j < other.digits.size() || add != 0; ++j) {
            uint64_t current = result.digits[i + j] + digits[i] * (j < other.digits.size() ? other.digits[j] : 0) + add;
            result.digits[i + j] = current % BASE;
            add = current / BASE;
        }
    }

    result.isNegative = (isNegative != other.isNegative);
    result.remove_leading_zeros();

    if (result.digits.size() == 1 && result.digits[0] == 0) {
        result.isNegative = false;
    }
    return result;
}

BigInt BigInt::operator/(const BigInt &other) const {
    if (other.is_zero()) {
        throw std::runtime_error("Division by zero");
    }

    BigInt divisor = other.abs();
    BigInt dividend = this->abs();
    BigInt result;
    result.digits.resize(dividend.digits.size(), 0);
    int res_pos = dividend.digits.size() - 1;

    if (dividend < divisor) {
        return BigInt(0);
    }
    if (dividend == divisor) {
        BigInt res(1);
        res.isNegative = (isNegative != other.isNegative);
        return res;
    }

    int n = dividend.digits.size();
    BigInt current;

    for (int i = n - 1; i >= 0; --i) {
        current.digits.insert(current.digits.begin(), digits[i]);
        current.remove_leading_zeros();

        uint64_t l = 0, r = BASE;
        while (l <= r) {
            uint64_t m = (l + r) / 2;
            BigInt tmp = BigInt(m) * divisor;
            if (tmp <= current) {
                l = m + 1;
            } else {
                r = m - 1;
            }
        }

        result.digits[res_pos] = r;
        if (r != 0) {
            current = current - (BigInt(r) * divisor);
        }
        --res_pos;
    }
    result.isNegative = (isNegative != other.isNegative);
    result.remove_leading_zeros();
    return result;
}

BigInt& BigInt::operator++() {
    *this = *this + BigInt(1);
    return *this;
}

BigInt& BigInt::operator--() {
    *this = *this + BigInt(-1);
    return *this;
}

BigInt& BigInt::operator+=(const BigInt &other) {
    *this = *this + other;
    return *this;
}

BigInt& BigInt::operator-=(const BigInt &other) {
    *this = *this - other;
    return *this;
}

BigInt& BigInt::operator*=(const BigInt &other) {
    *this = *this * other;
    return *this;
}

BigInt& BigInt::operator/=(const BigInt &other) {
    *this = *this / other;
    return *this;
}

std::istream &operator>>(std::istream &is, BigInt &other) {
    std::string input;
    is >> input;
    other = BigInt(input);
    return is;
}

BigInt BigInt::operator%(const BigInt &other) const {
    BigInt quotient = *this / other;
    BigInt result = *this - (quotient * other);
    result.isNegative = false;
    return result;
}

BigInt BigInt::mod_exp(const BigInt &exp, const BigInt &mod) const {
    if (exp < BigInt(2)) {
        if (exp == BigInt(1)) {
            return *this % mod;
        }
        return BigInt(1);
    }
    BigInt result = mod_exp(exp / BigInt(2), mod);
    result = (result * result) % mod;
    if (exp.digits[0] % 2 == 1) {
        result = (result * *this) % mod;
    }
    return result;
}

BigInt BigInt::karatsuba_multiply(const BigInt &other) const {
    BigInt x = *this;
    BigInt y = other;

    if (x.digits.size() <= 1 || y.digits.size() <= 1) {
        return x * y;
    }

    std::size_t n = std::max(x.digits.size(), y.digits.size());
    std::size_t m = n / 2;

    BigInt x0, x1, y0, y1;

    x0.digits.assign(x.digits.begin(), x.digits.begin() + std::min(m, x.digits.size()));
    x1.digits.assign(x.digits.begin() + std::min(m, x.digits.size()), x.digits.end());

    y0.digits.assign(y.digits.begin(), y.digits.begin() + std::min(m, y.digits.size()));
    y1.digits.assign(y.digits.begin() + std::min(m, y.digits.size()), y.digits.end());

    x0.remove_leading_zeros();
    x1.remove_leading_zeros();
    y0.remove_leading_zeros();
    y1.remove_leading_zeros();

    BigInt z0 = x0.karatsuba_multiply(y0);
    BigInt z2 = x1.karatsuba_multiply(y1);
    BigInt z1 = (x0 + x1).karatsuba_multiply(y0 + y1) - z0 - z2;

    BigInt result;

    result.digits = std::vector<uint64_t>(z2.digits.size() + 2 * m, 0);
    std::copy(z2.digits.begin(), z2.digits.end(), result.digits.begin() + 2 * m);

    BigInt z1_shift;
    z1_shift.digits = std::vector<uint64_t>(z1.digits.size() + m, 0);
    std::copy(z1.digits.begin(), z1.digits.end(), z1_shift.digits.begin() + m);

    result = result + z1_shift + z0;
    result.isNegative = (x.isNegative != y.isNegative);
    result.remove_leading_zeros();

    return result;
}

void BigInt::fft(std::vector<std::complex<long double>>& a, bool invert) {
    auto size = a.size();
    if (size == 1)  return;

    std::vector<std::complex<long double>> a0(size / 2);
    std::vector<std::complex<long double>> a1(size / 2);
    for (unsigned i = 0, j = 0; i < size; i += 2, j++) {
        a0[j] = a[i];
        a1[j] = a[i + 1];
    }

    fft(a0, invert);
    fft(a1, invert);

    long double ang = 2 * ((long double)(M_PI)) / size * (invert ? -1 : 1);
    std::complex<long double> w(1.0);
    std::complex<long double> wn(cosl(ang), sinl(ang));
    for (unsigned int i = 0; i < size / 2; ++i) {
        a[i] = a0[i] + w * a1[i];
        a[i + size / 2] = a0[i] - w * a1[i];
        if (invert) {
            a[i] /= 2;
            a[i + size / 2] /= 2;
        }
        w *= wn;
    }
}

std::string BigInt::to_string() const {
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

std::vector<uint64_t> BigInt::small_base_number(std::string& str) const {
    std::string temp = str;
    std::vector<uint64_t> res;
    if (temp.length() == 0) {
        res.push_back(0);
        return res;
    }

    for (long long i = temp.length(); i > 0; i -= 4) {
        if (i < 4) {
            res.push_back(atoi(temp.substr(0, i).c_str()));
        } else {
            res.push_back(atoi(temp.substr(i - 4, 4).c_str()));
        }
    }
    return res;
}

std::string BigInt::small_number_to_string(std::vector<uint64_t> &num) {
    std::string res;
    if (num.empty() || (num.back() == 0 && num.size() == 1)) {
        res += "0";
    } else {
        for (long long i = num.size() - 1; i >= 0; i--) {
            if (num[i] == 0) {
                res += "0000";
            } else {
                if (i != static_cast<long long>(num.size()) - 1) {
                    std::size_t len = num_length(num[i]);
                    if (len < 4) {
                        std::string s(4 - len, '0');
                        res += s;
                    }
                }
                res += std::to_string(num[i]);
            }
        }
    }
    return res;
}


BigInt BigInt::fft_multiply2(const BigInt& second) {
    std::string first_str = this->to_string();
    std::string second_str = second.to_string();

    std::vector<uint64_t> first_num = small_base_number(first_str);
    std::vector<uint64_t> second_num = small_base_number(second_str);
    std::vector<uint64_t> small_res;

    std::vector<std::complex<long double>> fa(first_num.begin(), first_num.end());
    std::vector<std::complex<long double>> fb(second_num.begin(), second_num.end());

    uint size = 1;
    while (size < std::max(first_num.size(), second_num.size())) {
        size <<= 1;
    }
    size <<= 1;
    fa.resize(size);
    fb.resize(size);

    fft(fa, false);
    fft(fb, false);

    for (uint i = 0; i < size; ++i) {
        fa[i] *= fb[i];
    }

    fft(fa, true);

    uint64_t carry = 0;
    for (size_t i = 0; i < size; ++i) {
        int64_t value = static_cast<int64_t>(std::round(fa[i].real())) + carry;
        small_res.push_back(value % SMALL_BASE);
        carry = value / SMALL_BASE;
    }

    if (carry > 0) {
        small_res.push_back(carry);
    }

    std::string small_res_str = small_number_to_string(small_res);
    BigInt res(small_res_str);
    res.remove_leading_zeros();
    res.isNegative = isNegative ^ second.isNegative;
    return res;
}