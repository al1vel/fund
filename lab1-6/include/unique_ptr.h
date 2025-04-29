#ifndef UNIQUE_PTR_H
#define UNIQUE_PTR_H
#include <memory>

namespace smart_ptr {
    template<class T, class Deleter = std::default_delete<T>>
    class Unique_ptr {
    private:
        Deleter deleter_ = Deleter();
        T *ptr_ = nullptr;

    public:
        Unique_ptr() : ptr_(nullptr) {
        }

        explicit Unique_ptr(T *ptr) : ptr_(ptr) {
        }

        Unique_ptr(const Unique_ptr&) = delete;

        Unique_ptr(Unique_ptr &&other) noexcept {
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
            deleter_ = other.deleter_;
        }

        ~Unique_ptr() {
            if (ptr_ != nullptr) {
                deleter_(ptr_);
            }
        }

        Unique_ptr& operator=(const Unique_ptr&) = delete;

        Unique_ptr &operator=(Unique_ptr &&other) noexcept {
            if (ptr_ != nullptr) {
                deleter_(ptr_);
            }
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
            deleter_ = other.deleter_;
            return *this;
        }

        T* get() const noexcept {
            return ptr_;
        }


    };
}

#endif //UNIQUE_PTR_H
