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

        [[nodiscard]] virtual std::size_t size() const = 0;

        [[nodiscard]] virtual std::size_t max_size() const = 0;

        [[nodiscard]] virtual bool empty() const = 0;
    };

    template<class T, class Allocator = std::allocator<T> >
    class Vector {
    private:
        using siz = std::size_t;
        T *data_ = nullptr;
        siz len = 0;
        siz capacity = 0;
        Allocator alloc = Allocator();

    public:
        Vector() = default;

        explicit Vector(siz n, T val = 0) {
            data_ = alloc.allocate(n * sizeof(T));
            capacity = n;
            len = n;
            for (int i = 0; i < n; i++) {
                data_[i] = val;
            }
        }

        Vector(std::initializer_list<T> init) {
            data_ = alloc.allocate(init.size() * sizeof(T));
            std::copy(init.begin(), init.end(), data_);
            len = init.size();
            capacity = init.size();
        }

        Vector(const Vector &other) {
            alloc = other.alloc;
            data_ = alloc.allocate(other.capacity * sizeof(T));
            std::copy(other.data_, other.data_ + other.capacity, data_);
            len = other.len;
            capacity = other.capacity;
        }

        Vector(Vector &&other) noexcept {
            alloc = other.alloc;
            data_ = other.data_;
            len = other.len;
            capacity = other.capacity;
            other.data_ = nullptr;
            other.len = 0;
            other.capacity = 0;
            other.data_ = nullptr;
        }

        ~Vector() {
            if (data_ != nullptr) {
                alloc.deallocate(data_, capacity * sizeof(T));
            }
        }

        Vector &operator=(const Vector &other) {
            if (this != &other) {
                if (data_ != nullptr) {
                    alloc.deallocate(data_, capacity * sizeof(T));
                }
                alloc = other.alloc;
                len = other.len;
                capacity = other.capacity;
                data_ = alloc.allocate(other.capacity * sizeof(T));
                std::copy(other.data_, other.data_ + other.capacity, data_);
            }
            return *this;
        }

        Vector &operator=(Vector &&other) noexcept {
            if (this != &other) {
                data_ = other.data_;
                alloc = other.alloc;
                len = other.len;
                capacity = other.capacity;
                other.data_ = nullptr;
                other.len = 0;
                other.capacity = 0;
            }
            return *this;
        }

        T& at(siz index) {
            if (index >= len) {
                throw std::out_of_range("At received invalid index.");
            }
            return data_[index];
        }

        T& operator[](siz index) {
            return data_[index];
        }

        T& front() {
            if (len == 0) {
                throw std::out_of_range("Front of empty vector.");
            }
            return data_[0];
        }

        T& back() {
            if (len == 0) {
                throw std::out_of_range("Back of empty vector.");
            }
            return data_[len - 1];
        }

        T* data() {
            return data_;
        }


    };
}

#endif //VECTOR_H
