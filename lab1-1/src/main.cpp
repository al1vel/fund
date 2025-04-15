#include <iostream>
#include "my_lib.h"

int main() {
    cont::Array<int, 10> ar;
    ar[0] = 1;
    ar[1] = 2;
    ar[2] = 3;
    for (const auto &x : ar) {
        std::cout << x << std::endl;
    }
    std::cout << "-----" << std::endl;

    cont::Array<int, 10>::ConstIterator iter = ar.cbegin();
    ++iter;
    iter++;

    std::cout << *iter << std::endl;
    std::cout << "-----" << std::endl;

    cont::Array<int, 10>::ConstReverseIterator iter2 = ar.crbegin();
    for (; iter2 != ar.crend(); ++iter2) {
        std::cout << *iter2 << std::endl;
    }

    std::cout << "-----" << std::endl;

    cont::Array<const int, 4> b = {1, 2,3,4};

    for (const auto &x : b) {
        std::cout << x << std::endl;
    }
    return 0;
}