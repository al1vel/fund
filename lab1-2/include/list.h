#ifndef LIST_H
#define LIST_H

#include <cstddef>

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
    class List {
    private:
        struct Node {
            T data;
            Node *next;
            Node *prev;
        };

        using NodeAllocator = std::allocator_traits<std::allocator<int> >::template rebind_alloc<Node>;
        NodeAllocator allocator;

        Node *head = nullptr;
        Node *tail = nullptr;
        std::size_t size = 0;

        void createHead(T val) {
            Node *head = allocator.allocate(sizeof(Node));
            head->data = val;
            head->next = nullptr;
            head->prev = nullptr;
            this->head = head;
            this->tail = head;
            this->size = 1;
        }

        Node *createNode(T val) {
            Node *node = allocator.allocate(sizeof(Node));
            node->data = val;
            node->next = nullptr;
            node->prev = nullptr;
            return node;
        }

        void pushBack(T val) {
            Node *node = createNode(val);
            tail->next = node;
            node->prev = tail;
            tail = node;
            size++;
        }

        void deleteList() {
            Node *node = head->next;
            while (node != nullptr) {
                allocator.deallocate(node, sizeof(Node));
                node = node->next;
            }
            allocator.deallocate(head, sizeof(Node));
            head = nullptr;
            tail = nullptr;
            size = 0;
        }

    public:
        List() {
            head = nullptr;
            tail = nullptr;
            size = 0;
        }

        List(std::initializer_list<T> init) {
            createHead(*(init.begin()));
            for (auto it = init.begin(); it != init.end(); ++it) {
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
            size = other.size;
            other.head = nullptr;
            other.tail = nullptr;
            other.size = 0;
        }

        ~List() {
            deleteList();
        }

        List &operator=(const List &other) {
            if (this != &other) {
                this->deleteList();
                this->allocator = other.allocator;
                createHead(other.head->data);
                while (other.head->next != nullptr) {
                    pushBack(other.head->next->data);
                    other.head = other.head->next;
                }
            }
            return *this;
        }

        List &operator=(List &&other) noexcept {
            if (this != &other) {
                this->allocator = other.allocator;
                this->head = other.head;
                this->tail = other.tail;
                this->size = other.size;
                other.size = 0;
                other.head = nullptr;
                other.tail = nullptr;
            }
            return *this;
        }

        T &front() {
            if (head == nullptr) {
                throw std::out_of_range("List::front. List doesn't exist.");
            }
            return head->data;
        }

        T &back() {
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

            ListIterator(const ListIterator &other) = default;

        public:
            ListIterator(ListIterator &&other) = default;

            ListIterator &operator++() {
                ptr = ptr->next;
                return *this;
            }

            ListIterator &operator--() {
                ptr = ptr->prev;
                return *this;
            }

            ListIterator &operator++(int) {
                ptr = ptr->next;
                return ptr->prev;
            }

            ListIterator &operator--(int) {
                ptr = ptr->prev;
                return ptr->next;
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

            ReverseListIterator &operator++(int) {
                ptr = ptr->prev;
                return ptr->next;
            }

            ReverseListIterator &operator--(int) {
                ptr = ptr->next;
                return ptr->prev;
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
            return ReverseIterator(nullptr);
        }

        ReverseIterator rend() const {
            return ReverseIterator(tail);
        }

        ConstReverseIterator crbegin() const {
            return ConstReverseIterator(nullptr);
        }

        ConstReverseIterator crend() const {
            return ConstReverseIterator(tail);
        }

        [[nodiscard]] bool empty() const {
            return size == 0;
        }

        [[nodiscard]] std::size_t Size() const {
            return size;
        }

        void clear() {
            deleteList();
        }


    };
}

#endif //LIST_H
