#ifndef DEQUE_H
#define DEQUE_H

#include "../../lab1-2/include/list.h"

namespace contDQ {
    template<class T, class Allocator = std::allocator<T>>
    class Deque : public cont::List<T, Allocator> {

    };
};

#endif //DEQUE_H