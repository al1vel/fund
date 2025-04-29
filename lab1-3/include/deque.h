#ifndef DEQUE_H
#define DEQUE_H

#include "../../lab1-2/include/list.h"
#include <iostream>

namespace contDQ {
    template<class T, class Allocator = std::allocator<T> >
    class Deque : public cont::List<T, Allocator> {
    private:
        cont::List<T, Allocator> list;

    public:
        Deque() {
            list = cont::List<T, Allocator>();
        }

        Deque(std::initializer_list<T> init) {
            list = cont::List<T, Allocator>(init);
        }

        Deque(const Deque &other) : cont::List<T, Allocator>(other.list) {
            list = other.list;
        }

        Deque(Deque &&other) noexcept {
            list = other.list;
        }

        ~Deque() override {
            list.clear();
        }

        Deque &operator=(const Deque &other) {
            if (this != &other) {
                list = other.list;
            }
            return *this;
        }

        Deque &operator=(Deque &&other) noexcept {
            if (this != &other) {
                list = other.list;
            }
            return *this;
        }

        T &at(std::size_t pos) {
            if (pos >= list.size()) {
                throw std::out_of_range("Deque::at. Pos is unreal.");
            }
            auto it = list.cbegin().next(pos);
            return *it;
        }

        T &operator[](std::size_t pos) {
            if (pos >= list.size()) {
                throw std::out_of_range("Deque::[]. Pos is unreal.");
            }
            auto it = list.cbegin();
            while(pos > 0) {
                ++it;
                pos--;
            }
            return *it;
        }

        T &front() override {
            return list.front();
        }

        T &back() override {
            return list.back();
        }

        using Iterator = cont::List<T, Allocator>::Iterator;
        using ConstIterator = cont::List<T, Allocator>::ConstIterator;
        using ReverseIterator = cont::List<T, Allocator>::ReverseIterator;
        using ConstReverseIterator = cont::List<T, Allocator>::ConstReverseIterator;

        Iterator begin() const override {
            return list.begin();
        }

        Iterator end() const override {
            return list.end();
        }

        ConstIterator cbegin() const override {
            return list.cbegin();
        }

        ConstIterator cend() const override {
            return list.cend();
        }

        ReverseIterator rbegin() const override {
            return list.rbegin();
        }

        ReverseIterator rend() const override {
            return list.rend();
        }

        ConstReverseIterator crbegin() const override {
            return list.crbegin();
        }

        ConstReverseIterator crend() const override {
            return list.crend();
        }

        [[nodiscard]] bool empty() const override {
            return list.empty();
        }

        [[nodiscard]] std::size_t size() const override {
            return list.size();
        }

        [[nodiscard]] std::size_t max_size() const override {
            return list.max_size();
        }

        void clear() override {
            list.clear();
        }

        void insert(ConstIterator posIter, T val) override {
            list.insert(posIter, val);
        }

        Iterator erase(ConstIterator posIter) override {
            return list.erase(posIter);
        }

        void push_back(T val) override {
            list.push_back(val);
        }

        void pop_back() override {
            list.pop_back();
        }

        void push_front(T val) override {
            list.push_front(val);
        }

        void pop_front() override {
            list.pop_front();
        }

        void resize(std::size_t newSize, T val) override {
            list.resize(newSize, val);
        }

        void swap(Deque &other) noexcept {
            list.swap(other.list);
        }

        bool operator==(const Deque &other) const {
            return list == other.list;
        }

        bool operator!=(const Deque &other) const {
            return !(*this == other);
        }

        std::strong_ordering operator<=>(const Deque &other) const {
            return list <=> other.list;
        }
    };
};

#endif //DEQUE_H
