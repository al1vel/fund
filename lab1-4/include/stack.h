#ifndef STACK_H
#define STACK_H

#include "../../lab1-3/include/deque.h"

namespace st {
    template<class T, class Container = contDQ::Deque<T> >
    class Stack {
    private:
        Container cont;

    public:
        Stack() {
            cont = Container();
        }
    };
}

#endif //STACK_H
