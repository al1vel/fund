#ifndef LIST_H
#define LIST_H

#include <cstddef>
#include <compare>
#include <memory>
#include <stdexcept>
#include <initializer_list>

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

    template<class T, class Allocator = std::allocator<T> >
    class List : public MyContainer<T> {
    private:
        using noConstT = std::remove_const_t<T>;
        struct Node {
            noConstT data;
            Node *next;
            Node *prev;
        };

        using NodeAllocator = std::allocator_traits<std::allocator<int> >::template rebind_alloc<Node>;
        NodeAllocator allocator;

        Node *head = nullptr;
        Node *tail = nullptr;
        std::size_t len = 0;

        void createHead(noConstT val) {
            Node *head = allocator.allocate(sizeof(Node));
            head->data = val;
            head->next = nullptr;
            head->prev = nullptr;
            this->head = head;
            this->tail = head;
            this->len = 1;
        }

        Node *createNode(noConstT val) {
            Node *node = allocator.allocate(sizeof(Node));
            node->data = val;
            node->next = nullptr;
            node->prev = nullptr;
            return node;
        }

        void pushBack(noConstT val) {
            Node *node = createNode(val);
            tail->next = node;
            node->prev = tail;
            tail = node;
            len++;
        }

        void deleteList() {
            while (head != nullptr) {
                Node *next = head->next;
                allocator.deallocate(head, sizeof(Node));
                head = next;
            }
            head = nullptr;
            tail = nullptr;
            len = 0;
        }

    public:
        List() {
            head = nullptr;
            tail = nullptr;
            len = 0;
        }

        List(std::initializer_list<T> init) {
            createHead(*(init.begin()));
            auto it = init.begin();
            ++it;
            for (; it != init.end(); ++it) {
                pushBack(*it);
            }
        }

        List(const List &other) {
            this->allocator = other.allocator;
            createHead(other.head->data);
            Node *p = other.head->next;
            while (p != nullptr) {
                pushBack(p->data);
                p = p->next;
            }
        }

        List(List &&other) noexcept {
            head = other.head;
            tail = other.tail;
            allocator = other.allocator;
            len = other.len;
            other.head = nullptr;
            other.tail = nullptr;
            other.len = 0;
        }

        ~List() {
            deleteList();
        }

        List &operator=(const List &other) {
            if (this != &other) {
                this->deleteList();
                this->allocator = other.allocator;
                createHead(other.head->data);
                Node *ptr = other.head->next;
                while (ptr != nullptr) {
                    pushBack(ptr->data);
                    ptr = ptr->next;
                }
            }
            return *this;
        }

        List &operator=(List &&other) noexcept {
            if (this != &other) {
                this->allocator = other.allocator;
                this->head = other.head;
                this->tail = other.tail;
                this->len = other.len;
                other.len = 0;
                other.head = nullptr;
                other.tail = nullptr;
            }
            return *this;
        }

        virtual T &front() {
            if (head == nullptr) {
                throw std::out_of_range("List::front. List doesn't exist.");
            }
            return head->data;
        }

        virtual T &back() {
            if (tail == nullptr) {
                throw std::out_of_range("List::back. List doesn't exist.");
            }
            return tail->data;
        }

        template<class IterType>
        class ListIterator {
        private:
            using noConstIterType = std::remove_const_t<IterType>;
            friend class List;
            noConstIterType *ptr = nullptr;

            explicit ListIterator(Node *ptr) : ptr(ptr) {
            }

        public:
            ListIterator(const ListIterator &other) = default;

            ListIterator(ListIterator &&other) = default;

            ListIterator &operator++() {
                ptr = ptr->next;
                return *this;
            }

            ListIterator &operator--() {
                ptr = ptr->prev;
                return *this;
            }

            ListIterator operator++(int) {
                ListIterator tmp = *this;
                ptr = ptr->next;
                return tmp;
            }

            ListIterator operator--(int) {
                ListIterator tmp = *this;
                ptr = ptr->prev;
                return tmp;
            }

            bool operator==(const ListIterator &other) const {
                return ptr == other.ptr;
            }

            bool operator!=(const ListIterator &other) const {
                return ptr != other.ptr;
            }

            T &operator*() {
                return ptr->data;
            }

            ListIterator &next(std::size_t offset = 1) {
                for (std::size_t i = 0; i < offset; i++) {
                    if (ptr == nullptr) {
                        throw std::out_of_range("ListIterator::next. Offset is out of the collection.");
                    }
                    ptr = ptr->next;
                }
                return *this;
            }
        };


        template<class IterType>
        class ReverseListIterator {
        private:
            friend class List;
            using noConstIterType = std::remove_const_t<IterType>;
            noConstIterType *ptr = nullptr;

            explicit ReverseListIterator(Node *ptr) : ptr(ptr) {
            }

            ReverseListIterator(const ReverseListIterator &other) = default;

        public:
            ReverseListIterator(ReverseListIterator &&other) = default;

            ReverseListIterator &operator++() {
                ptr = ptr->prev;
                return *this;
            }

            ReverseListIterator &operator--() {
                ptr = ptr->next;
                return *this;
            }

            ReverseListIterator operator++(int) {
                ReverseListIterator tmp = *this;
                ptr = ptr->prev;
                return tmp;
            }

            ReverseListIterator operator--(int) {
                ReverseListIterator tmp = *this;
                ptr = ptr->next;
                return tmp;
            }

            bool operator==(const ReverseListIterator &other) const {
                return ptr == other.ptr;
            }

            bool operator!=(const ReverseListIterator &other) const {
                return ptr != other.ptr;
            }

            T &operator*() {
                return ptr->data;
            }
        };

        using Iterator = ListIterator<Node>;
        using ReverseIterator = ReverseListIterator<Node>;
        using ConstIterator = ListIterator<const Node>;
        using ConstReverseIterator = ReverseListIterator<const Node>;

        Iterator begin() const {
            return Iterator(head);
        }

        Iterator end() const {
            return Iterator(nullptr);
        }

        ConstIterator cbegin() const {
            return ConstIterator(head);
        }

        ConstIterator cend() const {
            return ConstIterator(nullptr);
        }

        ReverseIterator rbegin() const {
            return ReverseIterator(tail);
        }

        ReverseIterator rend() const {
            return ReverseIterator(nullptr);
        }

        ConstReverseIterator crbegin() const {
            return ConstReverseIterator(tail);
        }

        ConstReverseIterator crend() const {
            return ConstReverseIterator(nullptr);
        }

        [[nodiscard]] bool empty() const {
            return len == 0;
        }

        [[nodiscard]] std::size_t size() const override  {
            return len;
        }

        [[nodiscard]] std::size_t max_size() const override {
            return len;
        }

        void clear() {
            deleteList();
        }

        void insert(ConstIterator posIter, noConstT val) {
            if (posIter.ptr == head) {
                Node* h = head;
                Node* newNode = createNode(val);
                newNode->next = h;
                h->prev = newNode;
                head = newNode;
                len++;
            } else {
                Node* whereToPut = posIter.ptr;
                Node* newNode = createNode(val);
                Node* prev = whereToPut->prev;
                prev->next = newNode;
                newNode->next = whereToPut;
                newNode->prev = prev;
                whereToPut->prev = newNode;
                len++;
            }
        }

        Iterator erase(ConstIterator posIter) {
            if (posIter.ptr == head) {
                Node* h = head->next;
                allocator.deallocate(posIter.ptr, sizeof(Node));
                head = h;
                head->prev = nullptr;
                len--;
                return Iterator(head);
            }
            Node* node = posIter.ptr;
            Node* next = node->next;
            Node* prev = node->prev;
            if (next != nullptr) {
                prev->next = next;
                next->prev = prev;
                allocator.deallocate(node, sizeof(Node));
                len--;
                return Iterator(next);
            }
            prev->next = nullptr;
            tail = prev;
            allocator.deallocate(node, sizeof(Node));
            len--;
            return Iterator(nullptr);
        }

        void push_back(noConstT val) {
            Node* node = createNode(val);
            if (head == nullptr) {
                head = node;
                tail = node;
                len = 1;
            } else {
                Node* last = tail;
                tail->next = node;
                tail = node;
                tail->prev = last;
                len++;
            }
        }

        void pop_back() {
            if (len == 0) {
                throw std::out_of_range("ListIterator::pop_back. Size is 0");
            }
            Node* rem = tail;
            tail = tail->prev;
            allocator.deallocate(rem, sizeof(Node));
            len--;
            tail->next = nullptr;
        }

        void push_front(noConstT val) {
            Node* node = createNode(val);
            if (head == nullptr) {
                head = node;
                tail = node;
                len = 1;
            } else {
                node->next = head;
                head->prev = node;
                head = node;
                len++;
            }
        }

        void pop_front() {
            if (len == 0) {
                throw std::out_of_range("ListIterator::pop_front. Size is 0");
            }
            Node* rem = head;
            head = head->next;
            allocator.deallocate(rem, sizeof(Node));
            len--;
            head->prev = nullptr;
        }

        void resize(std::size_t newSize, noConstT val = 0) {
            if (newSize <= 0) {
                throw std::out_of_range("ListIterator::resize. New size must be more than 0.");
            }
            if (len == newSize) {
                return;
            }
            if (len == 0) {
                createHead(val);
                while (len != newSize) {
                    push_back(val);
                }
                return;
            }
            if (len > newSize) {
                while (len != newSize) {
                    pop_back();
                }
                return;
            }
            if (len < newSize) {
                while (len != newSize) {
                    push_back(val);
                }
            }
        }

        void swap(List& other) noexcept {
            Node*tmpHead = this->head;
            this->head = other.head;
            other.head = tmpHead;

            Node* tmpTail = this->tail;
            this->tail = other.tail;
            other.tail = tmpTail;

            Allocator tmpAllocator = this->allocator;
            this->allocator = other.allocator;
            other.allocator = tmpAllocator;

            std::size_t tmpSize = this->len;
            this->len = other.len;
            other.len = tmpSize;
        }

        bool operator==(const MyContainer<T>& other) const override {
            const auto *otherList = dynamic_cast<const List *>(&other);
            if (this->len != otherList->len) {
                return false;
            }
            auto it1 = this->begin();
            auto it2 = otherList->begin();
            while (it1 != this->end()) {
                if (*it1 != *it2) {
                    return false;
                }
                ++it1;
                ++it2;
            }
            return true;
        }

        bool operator!=(const MyContainer<T>& other) const override {
            return !(*this == other);
        }

        std::strong_ordering operator<=>(const List& other) const {
            auto it1 = this->begin();
            auto it2 = other.begin();

            while (it1 != this->end() && it2 != other.end()) {
                if (auto cmp = *it1 <=> *it2; cmp != 0) {
                    return cmp;
                }
                ++it1;
                ++it2;
            }
            return std::strong_ordering::equal;
        }
    };
}

#endif //LIST_H