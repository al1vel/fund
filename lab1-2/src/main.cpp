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
    std::cout << 4 << " " << list4.empty() << " " << list4.size() << std::endl;

    std::cout << 5 << " ---- " << std::endl;

    for (auto it = list2.begin(); it != list2.end(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    list2.insert(list2.cbegin(), 10);
    //list2.insert(list2.cbegin(), 12);
    list2.insert(list2.cbegin().next(1), 12);

    for (auto it = list2.begin(); it != list2.end(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl << std::endl << "Erasing" << std::endl;

    cont::List<int> a = {1, 2, 3, 4, 5};
    auto iterRet = a.erase(a.cbegin());
    for (auto it = a.cbegin(); it != a.cend(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << *iterRet << std::endl;

    auto ii = a.erase(a.cbegin().next(2));
    for (auto it = a.cbegin(); it != a.cend(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << *ii << std::endl;

    a.erase(a.cbegin().next(2));
    for (auto it = a.cbegin(); it != a.cend(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;


    std::cout << std::endl << "Pop push test" << std::endl;
    cont::List<int> b;
    b.push_back(1);
    b.push_back(2);
    b.push_back(3);
    b.push_front(0);
    b.push_front(-1);
    b.push_front(-2);
    b.pop_front();
    b.pop_back();
    b.pop_back();
    b.push_back(2);

    for (auto it = b.cbegin(); it != b.cend(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << std::endl << "Resize" << std::endl;
    cont::List<int> c;
    c.resize(10, 1);
    for (auto it = c.cbegin(); it != c.cend(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    c.resize(5, 2);
    for (auto it = c.cbegin(); it != c.cend(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    c.resize(1, 3);
    for (auto it = c.cbegin(); it != c.cend(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    c.resize(3, 4);
    for (auto it = c.cbegin(); it != c.cend(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    c.resize(6);
    for (auto it = c.cbegin(); it != c.cend(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << std::endl << "Swap" << std::endl;
    cont::List<int> d{1,2,3}, e{10,11,12,13};

    auto it1 = d.begin();
    auto it2 = e.begin();

    d.swap(e);
    for (auto it = d.cbegin(); it != d.cend(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    for (auto it = e.cbegin(); it != e.cend(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << "d begin: " << *it1 << std::endl << "e begin: " << *it2 << std::endl;

    std::cout << std::endl << "Comp" << std::endl;
    cont::List<int> l1 = {1,2,10};
    cont::List<int> l2 = {1,2,0};
    cont::List<int> l3 = {1,2,10};

    std::cout << (l1 == l2) << " " << (l1 == l3) << " " << (l2 == l3) << std::endl;
    std::cout << (l1 < l2) << " " << (l1 > l2) << " " << (l1 <= l3) << std::endl;

    std::cout << std::endl << "Const" << std::endl;
    cont::List<int> cons = {3, 4, 5};
    cont::List<int>::ConstIterator cit = cons.cbegin();
    for (; cit != cons.cend(); cit++) {
        std::cout << *cit << " ";
    }
    std::cout << std::endl;

    cont::List<const int> consts = {3, 4, 5};
    for (auto it = consts.cbegin(); it != consts.cend(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

}

