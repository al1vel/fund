#include <iostream>
#include "list.h"

int main() {
    cont::List<int> list;
    //std::cout << list.front() << " " << list.back() << std::endl;

    cont::List<int> list2 = {1, 2, 3, 4, 5};
    std::cout << 1 << " " << list2.front() << " " << list2.back() << std::endl;

    cont::List<int> list3 = list2;
    std::cout << 2<< " " << list3.front() << " " << list3.back() << std::endl;

    cont::List<int> list4 = std::move(list3);
    std::cout << 3<< " " << list4.front() << " " << list4.back() << std::endl;

    list4.clear();
    std::cout << 4 << " " << list4.empty() << " " << list4.Size() << std::endl;

    std::cout << 5 << " ---- " << std::endl;

    for (auto it = list2.begin(); it != list2.end(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    list2.insert(list2.cbegin(), 10);
    list2.insert(list2.cbegin().next(1), 12);

    for (auto it = list2.begin(); it != list2.end(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}
