#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <compare>

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
        siz cap = 0;
        Allocator alloc = Allocator();

    public:
        Vector() = default;

        explicit Vector(siz n, T val = 0) {
            data_ = alloc.allocate(n * sizeof(T));
            cap = n;
            len = n;
            for (siz i = 0; i < n; i++) {
                data_[i] = val;
            }
        }

        Vector(std::initializer_list<T> init) {
            data_ = alloc.allocate(init.size() * sizeof(T));
            std::copy(init.begin(), init.end(), data_);
            len = init.size();
            cap = init.size();
        }

        Vector(const Vector &other) {
            alloc = other.alloc;
            data_ = alloc.allocate(other.cap * sizeof(T));
            std::copy(other.data_, other.data_ + other.cap, data_);
            len = other.len;
            cap = other.cap;
        }

        Vector(Vector &&other) noexcept {
            alloc = other.alloc;
            data_ = other.data_;
            len = other.len;
            cap = other.cap;
            other.data_ = nullptr;
            other.len = 0;
            other.cap = 0;
            other.data_ = nullptr;
        }

        ~Vector() {
            if (data_ != nullptr) {
                alloc.deallocate(data_, cap * sizeof(T));
            }
        }

        Vector &operator=(const Vector &other) {
            if (this != &other) {
                if (data_ != nullptr) {
                    alloc.deallocate(data_, cap * sizeof(T));
                }
                alloc = other.alloc;
                len = other.len;
                cap = other.cap;
                data_ = alloc.allocate(other.cap * sizeof(T));
                std::copy(other.data_, other.data_ + other.cap, data_);
            }
            return *this;
        }

        Vector &operator=(Vector &&other) noexcept {
            if (this != &other) {
                data_ = other.data_;
                alloc = other.alloc;
                len = other.len;
                cap = other.cap;
                other.data_ = nullptr;
                other.len = 0;
                other.cap = 0;
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

        [[nodiscard]] bool empty() const {
            return len == 0;
        }

        [[nodiscard]] siz size() const {
            return len;
        }

        [[nodiscard]] siz capacity() const {
            return cap;
        }

        void reserve(siz n) {
            if (n <= cap) {
                return;
            }
            T* new_data = alloc.allocate(n * sizeof(T));
            std::copy(data_, data_ + len, new_data);
            alloc.deallocate(data_, cap * sizeof(T));
            data_ = new_data;
            cap = n;
        }

        void shrink_to_fit() {
            T* new_data = alloc.allocate(len * sizeof(T));
            std::copy(data_, data_ + len, new_data);
            alloc.deallocate(data_, cap * sizeof(T));
            data_ = new_data;
            cap = len;
        }

        void clear() {
            if (len == 0) {
                throw(std::out_of_range("The vector is empty."));
            }
            for (siz i = 0; i < len; i++) {
                data_[i].~T();
            }
            len = 0;
        }

        void push_back(T val) {
            if (len == cap) {
                if (cap == 0) {
                    reserve(1);
                } else {
                    reserve(cap * 2);
                }
            }
            data_[len] = val;
            len++;
        }

        void pop_back() {
            if (len == 0) {
                throw std::out_of_range("Popped empty vector.");
            }
            std::allocator_traits<decltype(alloc)>::destroy(alloc, &data_[len - 1]);
            len--;
        }

        void insert(siz index, T val) {
            if (len == cap) {
                if (cap == 0) {
                    reserve(index + 1);
                    len = cap;
                } else {
                    reserve(cap * 2);
                    len++;
                }
            }
            for (siz i = len; i > index; --i) {
                data_[i] = data_[i - 1];
            }
            data_[index] = val;
        }

        void erase(siz index) {
            if (index >= len) {
                throw std::out_of_range("Erase index out of range.");
            }
            std::allocator_traits<decltype(alloc)>::destroy(alloc, &data_[index]);
            for (siz i = index; i < len - 1; ++i) {
                data_[i] = data_[i + 1];
            }
            len--;
        }

        void resize(siz new_size) {
            if (new_size > cap) {
                reserve(new_size);
            }
            if (new_size > len) {
                for (size_t i = len; i < new_size; ++i) {
                    data_[i] = 0;
                }
            } else if (new_size < len) {
                for (size_t i = new_size; i < len; ++i) {
                    data_[i].~T();
                }
            }
            len = new_size;
        }

        void swap(Vector &other) noexcept {
            Allocator tmpAlloc = alloc;
            alloc = other.alloc;
            other.alloc = tmpAlloc;

            T* tmpData = data_;
            data_ = other.data_;
            other.data_ = tmpData;

            siz tmpLen = len;
            len = other.len;
            other.len = tmpLen;

            siz tmpCap = cap;
            cap = other.cap;
            other.cap = tmpCap;
        }

        bool operator==(const Vector &other) const {
            if (len != other.len) {
                return false;
            }
            for (siz i = 0; i < len; ++i) {
                if (data_[i] != other.data_[i]) {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const Vector &other) const {
            return !(*this == other);
        }

        std::strong_ordering operator<=>(const Vector& other) const {
            size_t min_size = (len < other.len) ? len : other.len;

            for (size_t i = 0; i < min_size; ++i) {
                if (auto cmp = data_[i] <=> other.data_[i]; cmp != 0) {
                    return cmp;
                }
            }

            return len <=> other.len;
        }
    };
}

#endif //VECTOR_H