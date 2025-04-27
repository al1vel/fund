#ifndef STACK_H
#define STACK_H

#include "../../lab1-3/include/deque.h"
#include <iostream>

namespace st {
    template<class T, class Container = contDQ::Deque<T> >
    class Stack {
    private:
        Container cont;

    public:
        Stack() : cont() {}

        Stack(std::initializer_list<T> init) : cont(init) {}

        Stack(const Stack &other) : cont(other.cont) {}

        Stack(Stack &&other) noexcept : cont(std::move(other.cont)) {}

        ~Stack() {
            cont.clear();
        }

        Stack &operator=(const Stack &other) {
            if (this != &other) {
                cont.clear();
                cont = Container(other.cont);
            }
            return *this;
        }

        Stack &operator=(Stack &&other) noexcept {
            if (this != &other) {
                cont = other.cont;
                other.cont = Container();
            }
            return *this;
        }

        T &top() {
            return cont.back();
        }

        [[nodiscard]] bool empty() const {
            return cont.empty();
        }

        [[nodiscard]] size_t size() const {
            return cont.size();
        }

        void push(const T &val) {
            cont.push_back(val);
        }

        void pop() {
            cont.pop_back();
        }

        void swap(Stack &other) noexcept {
            this->cont.swap(other.cont);
        }

        bool operator==(const Stack &other) const {
            return cont == other.cont;
        }

        bool operator!=(const Stack &other) const {
            return !(*this == other);
        }

        std::strong_ordering operator<=>(const Stack &other) const {
            return cont <=> other.cont;
        }
    };
}

#endif //STACK_H
