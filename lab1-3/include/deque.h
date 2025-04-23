#ifndef DEQUE_H
#define DEQUE_H

#include "../../lab1-2/include/list.h"

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

        Deque(const Deque &other) {
            list = other.list;
        }

        Deque(Deque &&other) noexcept {
            list = other.list;
            other.list = 0;
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
                other.list = 0;
            }
            return *this;
        }

        T &at(std::size_t pos) {
            if (pos >= list.size()) {
                throw std::out_of_range("Deque::at. Pos is unreal.");
            }
            auto it = list.cbegin().next(pos - 1);
            return *it;
        }

        T &operator[](std::size_t pos) {
            auto it = list.cbegin();
            for (; it != list.cend(); ++it) {
                if (it.ptr == nullptr) {
                    return nullptr;
                }
                ++it;
            }
            return *it;
        }

        T &front() override {
            return list.front();
        }

        T &back() override {
            return list.back();
        }

        template<class IterType>
        class DequeIterator {
        private:
            using noConstIterType = std::remove_const_t<IterType>;
            noConstIterType *dq_ptr = nullptr;

            explicit DequeIterator(noConstIterType *dq_ptr) : dq_ptr(dq_ptr) {
            }

        public:
            DequeIterator(const DequeIterator& other) = default;
            DequeIterator(const DequeIterator&& other) = default;

            DequeIterator& operator++() {

            }
        };
    };
};

#endif //DEQUE_H
