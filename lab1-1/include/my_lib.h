#ifndef MY_LIB_H
#define MY_LIB_H

#include <cstddef>
#include <stdexcept>
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


    template<class T, std::size_t N>
    class Array : public MyContainer<T> {
    protected:
        std::size_t cap = 0;
        T *data = nullptr;

        class ArrayIterator {
        protected:
            friend class Array;
            T *ptr = nullptr;

            explicit ArrayIterator(T *ptr) : ptr(ptr) {
            }

            ArrayIterator(const ArrayIterator &other) = default;

        public:
            ArrayIterator(ArrayIterator &&other) = default;

            ArrayIterator &operator++() {
                ++ptr;
                return *this;
            }

            ArrayIterator operator++(int) {
                ArrayIterator tmp = *this;
                ++this->ptr;
                return tmp;
            }

            ArrayIterator &operator--() {
                --ptr;
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
                return !(this->ptr == other.ptr);
            };

            T operator*() {
                return *this->ptr;
            }
        };

        class ReverseArrayIterator {
        protected:
            friend class Array;
            T *ptr = nullptr;

            ReverseArrayIterator(const ReverseArrayIterator &other) = default;

            explicit ReverseArrayIterator(T *ptr) : ptr(ptr) {
            }

        public:
            ReverseArrayIterator(ReverseArrayIterator &&other) = default;

            ReverseArrayIterator &operator++() {
                --ptr;
                return *this;
            }

            ReverseArrayIterator operator++(int) {
                ReverseArrayIterator tmp = *this;
                --this->ptr;
                return tmp;
            }

            ReverseArrayIterator &operator--() {
                ++ptr;
                return *this;
            }

            ReverseArrayIterator operator--(int) {
                ReverseArrayIterator tmp = *this;
                ++this->ptr;
                return tmp;
            }

            bool operator==(const ArrayIterator &other) const {
                return this->ptr == other.ptr;
            }

            bool operator!=(const ArrayIterator &other) const {
                return !(this->ptr == other.ptr);
            };

            T operator*() {
                return *this->ptr;
            }
        };

    public:
        using Iterator = ArrayIterator;
        using ReverseIterator = ReverseArrayIterator;
        using ConstIterator = const ArrayIterator;
        using ConstReverseIterator = const ReverseArrayIterator;


        Array() {
            this->data = new T[N];
            this->cap = N;
        }

        Array(std::initializer_list<T> init) {
            if (init.size() != N) {
                throw std::invalid_argument("Incorrect initializer list size.");
            }
            this->data = new T[N];
            std::copy(init.begin(), init.end(), this->data);
            this->cap = N;
        }

        Array(Array &other) {
            cap = other.cap;
            data = new T[cap];
            std::copy(other.data, other.data + cap, this->data);
        }

        Array(Array &&other) noexcept {
            this->cap = other.cap;
            this->data = other.data;
        }

        ~Array() override {
            delete[] this->data;
        }

        Array &operator=(const Array &other) {
            if (this != &other) {
                if (this->cap != other.cap) {
                    throw std::invalid_argument("Incorrect size.");
                }
                std::copy(other.data, other.data + this->cap, this->data);
            }
            return *this;
        }

        T &at(std::size_t ind) {
            if (ind >= this->cap) {
                throw std::out_of_range("Incorrect index.");
            }
            return this->data[ind];
        }

        T &operator[](std::size_t ind) {
            return this->data[ind];
        }

        T &front() {
            return this->data[0];
        }

        T &back() {
            return this->data[this->cap - 1];
        }

        T* Data() {
            return this->data;
        }

        Iterator begin() const noexcept {
            return Iterator(this->data);
        }

        Iterator end() const noexcept {
            return Iterator(this->data + this->cap);
        }

        ConstIterator cbegin() const noexcept {
            return ConstIterator(this->data);
        }

        ConstIterator cend() const noexcept {
            return ConstIterator(this->data + this->cap);
        }

        ReverseIterator rbegin() const noexcept {
            return ReverseIterator(this->data + this->cap - 1);
        }

        ReverseIterator rend() const noexcept {
            return ReverseIterator(this->data - 1);
        }

        ConstReverseIterator crbegin() const noexcept {
            return ConstReverseIterator(this->data + this->cap - 1);
        }

        ConstReverseIterator crend() const noexcept {
            return ConstReverseIterator(this->data - 1);
        }

        [[nodiscard]] bool empty() const noexcept override {
            return this->cap == 0;
        }

        [[nodiscard]] std::size_t size() const noexcept override {
            return this->cap;
        }

        [[nodiscard]] std::size_t max_size() const noexcept override {
            return this->cap;
        }

        void fill(T val) {
            for (std::size_t i = 0; i < this->cap; i++) {
                this->data[i] = val;
            }
        }

        void swap(Array &other) noexcept {
            for (std::size_t i = 0; i < this->cap; i++) {
                T tmp = this->data[i];
                this->data[i] = other.data[i];
                other.data[i] = tmp;
            }
        }

        bool operator==(const MyContainer<T>& other) const override {
            const auto* otherArr = dynamic_cast<const Array*>(&other);
            if (this->cap != otherArr->cap) {
                return false;
            }
            for (std::size_t i = 0; i < this->cap; i++) {
                if (this->data[i] != otherArr->data[i]) {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const MyContainer<T>& other) const override {
            return !(*this == other);
        }

        std::strong_ordering operator<=>(const Array& other) const {
            if (auto cmp = this.cap <=> other.cap; cmp != 0) {
                return cmp;
            }

            for (std::size_t i = 0; i < this->cap; i++) {
                if (auto cmp = this.data[i] <=> other.data[i]; cmp != 0) {
                    return cmp;
                }
            }
            return std::strong_ordering::equal;
        }
    };
}

#endif //MY_LIB_H
