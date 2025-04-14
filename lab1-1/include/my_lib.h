#ifndef MY_LIB_H
#define MY_LIB_H

#include <cstddef>

template<class T>
class MyContainer {
public:
    MyContainer() = default;

    MyContainer(const MyContainer &other) = default;

    virtual ~MyContainer() = default;
};

template<class T, std::size_t N>
class Array : public MyContainer<T> {
protected:
    std::size_t cap = 0;
    T *arr = nullptr;

    class ArrayIterator {
    protected:
        friend class Array;
        T *ptr = nullptr;

        ArrayIterator(T *ptr) : ptr(ptr) {
        }

        ArrayIterator(const ArrayIterator &other) = default;

    public:
        ArrayIterator(ArrayIterator &&other) = default;

        ArrayIterator operator++() {
            this->ptr++;
            return *this;
        }

        ArrayIterator operator++(int) {
            ArrayIterator tmp = *this;
            ++this->ptr;
            return tmp;
        }

        ArrayIterator operator--() {
            this->ptr--;
            return *this;
        }

        ArrayIterator operator--(int) {
            ArrayIterator tmp = *this;
            --this->ptr;
            return tmp;
        }

        bool operator==(const ArrayIterator &other) const {
            return this->ptr == other.ptr;
        }

        bool operator!=(const ArrayIterator &other) const {
            return !(*this == other);
        };

        T operator*() {
            return *this->ptr;
        }
    };

public:
    using Iterator = ArrayIterator;

    Array() : cap(20), arr(new T[this->cap]) {
    }

    explicit Array(std::size_t capacity) : cap(capacity), arr(new T[this->cap]) {
    }

    Array(Array &other) {
        cap = other.cap;
        arr = new T[other.cap];
    }

    Array(Array &&other) noexcept {
        this->cap = other.cap;
        this->arr = other.arr;
    }

    ~Array() {
        delete[] this->arr;
    }

    T &operator[](std::size_t ind) {
        return this->arr[ind];
    }

    std::size_t size() const {
        return this->cap;
    }

    Iterator begin() {
        return Iterator(this->arr);
    }

    Iterator end() {
        return Iterator(this->arr + this->cap);
    }
};

#endif //MY_LIB_H
