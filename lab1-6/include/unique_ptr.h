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

        T& operator*() const {
            return *ptr_;
        }

        T* operator->() const {
            return ptr_;
        }

        explicit operator bool() const noexcept {
            return ptr_ != nullptr;
        }

        void reset(T* ptr = nullptr) noexcept {
            if (ptr_ != nullptr) {
                deleter_(ptr_);
            }
            ptr_ = ptr;
        }

        T* release() noexcept {
            T *tmp = ptr_;
            ptr_ = nullptr;
            return tmp;
        }

        void swap(Unique_ptr& other) noexcept {
            T *tmp = ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = tmp;

            Deleter tmp_deleter = deleter_;
            deleter_ = other.deleter_;
            other.deleter_ = tmp_deleter;
        }
    };

    template<class T, class Deleter>
    class Unique_ptr<T[], Deleter> {
    private:
        Deleter arr_deleter_ = Deleter();
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
            arr_deleter_ = other.arr_deleter_;
        }

        ~Unique_ptr() {
            if (ptr_ != nullptr) {
                arr_deleter_(ptr_);
            }
        }

        Unique_ptr& operator=(const Unique_ptr&) = delete;

        Unique_ptr &operator=(Unique_ptr &&other) noexcept {
            if (ptr_ != nullptr) {
                arr_deleter_(ptr_);
            }
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
            arr_deleter_ = other.arr_deleter_;
            return *this;
        }

        T* get() const noexcept {
            return ptr_;
        }

        T& operator[](size_t index) const {
            return ptr_[index];
        }

        explicit operator bool() const noexcept {
            return ptr_ != nullptr;
        }

        void reset(T* ptr = nullptr) noexcept {
            if (ptr_ != nullptr) {
                arr_deleter_(ptr_);
            }
            ptr_ = ptr;
        }

        T* release() noexcept {
            T *tmp = ptr_;
            ptr_ = nullptr;
            return tmp;
        }

        void swap(Unique_ptr& other) noexcept {
            T *tmp = ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = tmp;

            Deleter tmp_deleter = arr_deleter_;
            arr_deleter_ = other.arr_deleter_;
            other.arr_deleter_ = tmp_deleter;
        }
    };
}

#endif //UNIQUE_PTR_H
