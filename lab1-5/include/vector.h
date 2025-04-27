#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>

namespace cont {
    template<class T>
    class MyContainer {
    public:
        MyContainer() = default;

        MyContainer(const MyContainer &other) = default;

        virtual ~MyContainer() = default;

        MyContainer &operator=(const MyContainer &other) = default;

        virtual bool operator==(const MyContainer &other) const = 0;

        virtual bool operator!=(const MyContainer &other) const = 0;

        [[nodiscard]] virtual std::size_t size() const  = 0;

        [[nodiscard]] virtual std::size_t max_size() const = 0;

        [[nodiscard]] virtual bool empty() const = 0;
    };

    template<class T, class Allocator = std::allocator<T>>
    class Vector {
    private:
        using siz = std::size_t;
        T* data = nullptr;
        siz len = 0;
        siz capacity = 0;
    public:
        Vector() = default;
    };
}

#endif //VECTOR_H
