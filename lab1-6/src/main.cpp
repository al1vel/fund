#include "unique_ptr.h"
#include <iostream>

int main() {
    smart_ptr::Unique_ptr<int> p(new int(10));
    std::cout << *p << std::endl;

    smart_ptr::Unique_ptr<int[], std::default_delete<int[]>> p2(new int[10]);
    for (int i = 0; i < 10; i++) {
        p2[i] = i;
    }

    for (int i = 0; i < 10; i++) {
        std::cout << p2[i] << std::endl;
    }
}