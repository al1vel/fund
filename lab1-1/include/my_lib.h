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
        template<class Type, std::size_t>
        friend class Array;

    protected:
        using NoConstT = std::remove_const_t<T>;
        std::size_t cap = 0;
        NoConstT *data = nullptr;

        template<class IterType>
        class ArrayIterator {
        protected:
            friend class Array;
            using NoConstIterType = std::remove_const_t<IterType>;
            NoConstIterType *ptr = nullptr;

            explicit ArrayIterator(NoConstT *ptr) : ptr(ptr) {
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
            }

            const T& operator*() {
                return *this->ptr;
            }
        };

        template<class IterType>
        class ReverseArrayIterator {
        protected:
            friend class Array;
            using NoConstIterType = std::remove_const_t<IterType>;
            NoConstIterType *ptr = nullptr;

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

            bool operator==(const ReverseArrayIterator &other) const {
                return this->ptr == other.ptr;
            }

            bool operator!=(const ReverseArrayIterator &other) const {
                return !(this->ptr == other.ptr);
            };

            T operator*() {
                return *this->ptr;
            }
        };

    public:
        using Iterator = ArrayIterator<NoConstT>;
        using ReverseIterator = ReverseArrayIterator<NoConstT>;
        using ConstIterator = ArrayIterator<const NoConstT>;
        using ConstReverseIterator = ReverseArrayIterator<const NoConstT>;


        Array() {
            this->data = new NoConstT[N];
            this->cap = N;
            for (std::size_t i = 0; i < N; ++i) {
                this->data[i] = 0;
            }
        }

        Array(std::initializer_list<NoConstT> init) {
            if (init.size() != N) {
                throw std::invalid_argument("Incorrect initializer list size.");
            }
            this->data = new NoConstT[N];
            std::copy(init.begin(), init.end(), this->data);
            this->cap = N;
        }

        Array(const Array &other) {
            cap = other.cap;
            data = new NoConstT[cap];
            std::copy(other.data, other.data + cap, this->data);
        }

        Array(Array &&other) noexcept {
            this->cap = other.cap;
            this->data = other.data;
            other.data = nullptr;
            other.cap = 0;
        }

        ~Array() override {
            delete[] this->data;
        }

        Array &operator=(const Array &other) {
            if (this != &other) {
                if (this->data != other.data) {
                    if (this->cap != other.cap) {
                        this->cap = other.cap;
                        delete[] this->data;
                        this->data = new T[this->cap];
                        std::copy(other.data, other.data + this->cap, this->data);
                    }
                    std::copy(other.data, other.data + this->cap, this->data);
                }
            }
            return *this;
        }

        Array &operator=(Array &&other) noexcept {
            this->cap = other.cap;
            this->data = other.data;
            other.data = nullptr;
            other.cap = 0;
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

        T *Data() {
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

        void fill(NoConstT val) {
            for (std::size_t i = 0; i < this->cap; i++) {
                this->data[i] = val;
            }
        }

        void swap(Array &other) noexcept {
            for (std::size_t i = 0; i < this->cap; i++) {
                NoConstT tmp = this->data[i];
                this->data[i] = other.data[i];
                other.data[i] = tmp;
            }
        }

        template<std::size_t N2>
        std::strong_ordering operator<=>(const Array<T, N2> &other) const {
            std::size_t n = std::min(N, N2);
            for (std::size_t i = 0; i < n; i++) {
                if (auto cmp = this->data[i] <=> other.data[i]; cmp != 0) {
                    return cmp;
                }
            }

            if (auto cmp = this->cap <=> other.cap; cmp != 0) {
                return cmp;
            }

            return std::strong_ordering::equal;
        }

        bool operator==(const MyContainer<T> &other) const override {
            const auto *otherArr = dynamic_cast<const Array *>(&other);
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

        bool operator!=(const MyContainer<T> &other) const override {
            return !(*this == other);
        }
    };
}

#endif //MY_LIB_H
