#include "big_int.h"

int main() {
    BigInt bigint;
    BigInt bigint2(1234567);
    BigInt bigint3(-1234567891234);
    BigInt bigint4("112211221122112211223344556677");

    std::cout << bigint << std::endl;
    std::cout << bigint2 << std::endl;
    std::cout << bigint3 << std::endl;
    std::cout << bigint4 << std::endl;
}