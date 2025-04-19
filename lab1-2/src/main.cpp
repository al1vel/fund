#include <iostream>
#include "list.h"

int main() {
    cont::List<int> list;
    //std::cout << list.front() << " " << list.back() << std::endl;

    cont::List<int> list2 = {1, 2, 3, 4, 5};
    std::cout << list2.front() << " " << list2.back() << std::endl;

    cont::List<int> list3 = list2;
    std::cout << list3.front() << " " << list3.back() << std::endl;

    cont::List<int> list4 = std::move(list3);
    std::cout << list4.front() << " " << list4.back() << std::endl;
}
