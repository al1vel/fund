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
}