//
// Created by artemiy on 12.05.2020.
//

#include <bits/allocator.h>
#include <memory>

#ifndef ARCHRONIS_LIST_HPP
#define ARCHRONIS_LIST_HPP

template<typename T, typename Allocator = std::allocator<T>>
class list {
private:

    struct Node {
        T data_;
        mutable Node *next_ = nullptr;
        mutable Node *prev_ = nullptr;


        explicit Node(const T &data);

        explicit Node(T &&data) noexcept;

        template<typename ...Args>
        Node(Args &&...args);

        ~Node() = default;
    };

    struct Border {
        char fictitious_memory[sizeof(T)];
        mutable Node *next_ = nullptr;
        mutable Node *prev_ = nullptr;
    };

    using BorderAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<Border>;
    using NodeAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using Border_traits = std::allocator_traits<BorderAlloc>;
    using Node_traits = std::allocator_traits<NodeAlloc>;
    using Type_traits = std::allocator_traits<Allocator>;


public:

    struct Iter;


    using iterator = Iter;

    explicit list(Allocator alloc = Allocator());

    ~list();

    void push_back(const T &);

    void push_back(T &&);

    void push_front(const T &);

    void push_front(T &&);

    void pop_back();

    void pop_front();

    void clear();

    T front();

    T back();

    template<typename ...Args>
    Iter emplace(const Iter &iter, Args &&...args);

    Iter insert(const Iter &iter, const T &);

    Iter insert(const Iter &iter, T &&);

    Iter erase(const Iter &iter);

//    template<typename ...Args>
//    void emplace_back(Args &&...args);
//
//    template<typename ...Args>
//    void emplace_front(Args &&...args);


    unsigned size() const;


    Iter begin() const;

    Iter end()const;

    //const Iter cbegin() const;

    //const Iter cend() const;


private:
    unsigned size_;
    Node *rend_;
    Node *end_;
    Node *head_ = nullptr;
    Node *tail_ = nullptr;
    NodeAlloc node_alloc_;
    BorderAlloc border_alloc;

};

template<typename T, typename Allocator>
list<T, Allocator>::Node::Node(const T &data):
        data_(data),
        prev_(nullptr),
        next_(nullptr) {}

template<typename T, typename Allocator>
list<T, Allocator>::Node::Node(T &&data) noexcept:
        data_(data),
        prev_(nullptr),
        next_(nullptr) {}

template<typename T, typename Allocator>
template<typename... Args>
list<T, Allocator>::Node::Node(Args &&... args):
        prev_(nullptr),
        next_(nullptr),
        data_(std::forward<Args>(args)...) {}


template<typename T, typename Allocator>
struct list<T, Allocator>::Iter {
public:
    friend list;

    T &operator*();

    const T &operator*() const;

    T *operator->();

    const T *operator->() const;

    Iter& operator++();

    const Iter operator++(int);

    Iter& operator--();

    const Iter operator--(int);


    Iter(const Iter &other);

    Iter(Iter &&other) noexcept;

    Iter() = default;

    Iter &operator=(const Iter &other);

    Iter &operator=(Iter &&other) noexcept;

    bool operator==(const Iter &other) const;

    bool operator!=(const Iter &other) const;

private:


    Node *ptr_ = nullptr;
};


template<typename T, typename Allocator>
list<T, Allocator>::Iter::Iter(const list<T, Allocator>::Iter &other):
        ptr_(other.ptr_) {}

template<typename T, typename Allocator>
list<T, Allocator>::Iter::Iter(list<T, Allocator>::Iter &&other) noexcept:
        ptr_(other.ptr_) {

    other.ptr_ = nullptr;
}

template<typename T, typename Allocator>
typename list<T, Allocator>::Iter &list<T, Allocator>::Iter::operator=(const list<T, Allocator>::Iter &other) {
    if (*this != other) {
        this->ptr_ = other.ptr_;
    }
    return *this;
}

template<typename T, typename Allocator>
typename list<T, Allocator>::Iter &list<T, Allocator>::Iter::operator=(list<T, Allocator>::Iter &&other) noexcept {
    if (*this != other) {
        this->ptr_ = other.ptr_;
        other.ptr_ = nullptr;
    }
    return *this;
}

template<typename T, typename Allocator>
T &list<T, Allocator>::Iter::operator*() {
    return ptr_->data_;
}

template<typename T, typename Allocator>
T *list<T, Allocator>::Iter::operator->() {
    return &ptr_->data_;
}

template<typename T, typename Allocator>
typename list<T, Allocator>::Iter& list<T, Allocator>::Iter::operator++() {
    ptr_ = ptr_->next_;
    return *this;
}

template<typename T, typename Allocator>
const typename list<T, Allocator>::Iter list<T, Allocator>::Iter::operator++(int) {
    Iter copy = *this;
    this->ptr_ = ptr_->next_;
    return copy;
}

template<typename T, typename Allocator>
typename list<T, Allocator>::Iter& list<T, Allocator>::Iter::operator--() {
    ptr_ = ptr_->prev_;
    return *this;
}

template<typename T, typename Allocator>
const typename list<T, Allocator>::Iter list<T, Allocator>::Iter::operator--(int) {
    Iter copy = *this;
    ptr_ = ptr_->prev_;
    return copy;
}

template<typename T, typename Allocator>
bool list<T, Allocator>::Iter::operator==(const list<T, Allocator>::Iter &other) const {
    return other.ptr_ == ptr_;
}

template<typename T, typename Allocator>
bool list<T, Allocator>::Iter::operator!=(const list<T, Allocator>::Iter &other) const {
    return other.ptr_ != ptr_;
}

template<typename T, typename Allocator>
const T &list<T, Allocator>::Iter::operator*() const {
    return ptr_->data_;
}

template<typename T, typename Allocator>
const T *list<T, Allocator>::Iter::operator->() const {
    return &ptr_->data_;
}
//***********************************************************************************************

template<typename T, typename Allocator>
list<T, Allocator>::list(Allocator alloc):
        size_(0),
        node_alloc_(NodeAlloc()) {
    end_ = reinterpret_cast<Node *>(Border_traits::allocate(border_alloc, 1));//T may not have default constructor
    rend_ = reinterpret_cast<Node *>(Border_traits::allocate(border_alloc, 1));
    Border_traits::construct(border_alloc, reinterpret_cast<Border*>(end_));
    Border_traits::construct(border_alloc, reinterpret_cast<Border*>(rend_));
    rend_->next_ = end_;
    end_->prev_ = rend_;
}

template<typename T, typename Allocator>
void list<T, Allocator>::push_back(const T &data) {
    Node *ptr = Node_traits::allocate(node_alloc_, 1);
    Node_traits::construct(node_alloc_, ptr, data);
    //
    if (size_) {
        tail_->next_ = ptr;
        ptr->prev_ = tail_;
        tail_ = ptr;
    } else {
        head_ = tail_ = ptr;
        head_->prev_ = rend_;
        rend_->next_ = head_;
    }
    //
    tail_->next_ = end_;
    end_->prev_ = tail_;
    ++size_;
}

template<typename T, typename Allocator>
void list<T, Allocator>::push_back(T &&data) {
    Node *ptr = Node_traits::allocate(node_alloc_, 1);
    Node_traits::construct(node_alloc_, ptr, data);
    //
    if (size_) {
        tail_->next_ = ptr;
        ptr->prev_ = tail_;
        tail_ = ptr;
    } else {
        head_ = tail_ = ptr;
        head_->prev_ = rend_;
        rend_->next_ = head_;
    }
    //
    end_->prev_ = tail_;
    tail_->next_ = end_;
    ++size_;
}

template<typename T, typename Allocator>
void list<T, Allocator>::push_front(const T &data) {
    Node *ptr = Node_traits::allocate(node_alloc_, 1);
    Node_traits::construct(node_alloc_, ptr, data);
    //
    if (size_) {
        head_->prev_ = ptr;
        ptr->next_ = head_;
        head_ = ptr;
    } else {
        head_ = tail_ = ptr;
        tail_->next_ = end_;
        end_->prev_ = tail_;
    }
    //
    head_->prev_ = rend_;
    rend_->next_ = head_;
    ++size_;
}

template<typename T, typename Allocator>
void list<T, Allocator>::push_front(T &&data) {
    Node *ptr = Node_traits::allocate(node_alloc_, 1);
    Node_traits::construct(node_alloc_, ptr, data);
    //
    if (size_) {
        head_->prev_ = ptr;
        ptr->next_ = head_;
        head_ = ptr;
    } else {
        head_ = tail_ = ptr;
        tail_->next_ = end_;
        end_->prev_ = tail_;
    }
    //
    head_->prev_ = rend_;
    rend_->next_ = head_;
    ++size_;
}

template<typename T, typename Allocator>
void list<T, Allocator>::pop_back() {
    Node *pv = tail_;
    tail_ = tail_->prev_;
    tail_->next_ = end_;
    //end_->prev_ = tail_;
    Node_traits::destroy(node_alloc_, pv);
    Node_traits::deallocate(node_alloc_, pv, 1);
    --size_;
}

template<typename T, typename Allocator>
void list<T, Allocator>::pop_front() {
    Node *pv = head_;
    head_ = head_->next_;
    head_->prev_ = rend_;
    rend_->next_ = head_;
    Node_traits::destroy(node_alloc_, pv);
    Node_traits::deallocate(node_alloc_, pv, 1);
    --size_;
}

template<typename T, typename Allocator>
template<typename... Args>
typename list<T, Allocator>::Iter list<T, Allocator>::emplace(const list::Iter &iter, Args &&... args) {
    Node *ptr = Node_traits::allocate(node_alloc_, 1);
    Node_traits::construct(node_alloc_, ptr, std::forward<Args>(args)...);
    if (size_ == 0) {
        tail_ = head_ = ptr;
    }
    if (iter.ptr_ == head_) {
        head_ = ptr;
    }
    Node *prev = iter.ptr_->prev_;
    prev->next_ = ptr;
    ptr->prev_ = prev;
    ptr->next_ = iter.ptr_;
    iter.ptr_->prev_ = ptr;
    Iter iter_to_return;
    iter_to_return.ptr_ = ptr;
    ++size_;
    return iter_to_return;
}

template<typename T, typename Allocator>
typename list<T, Allocator>::Iter list<T, Allocator>::insert(const list::Iter &iter, const T &data) {
    Node *ptr = Node_traits::allocate(node_alloc_, 1);
    Node_traits::construct(node_alloc_, ptr, data);
    if (size_ == 0) {
        tail_ = head_ = ptr;
    }
    if (iter.ptr_ == head_) {
        head_ = ptr;
    }
    Node *prev = iter.ptr_->prev_;
    prev->next_ = ptr;
    ptr->prev_ = prev;
    ptr->next_ = iter.ptr_;
    iter.ptr_->prev_ = ptr;
    Iter iter_to_return;
    iter_to_return.ptr_ = ptr;
    ++size_;
    return iter_to_return;
}

template<typename T, typename Allocator>
unsigned list<T, Allocator>::size() const {
    return size_;
}

template<typename T, typename Allocator>
typename list<T, Allocator>::Iter list<T, Allocator>::begin() const {
    Iter begin;
    if (size_){
        begin.ptr_ = head_;
    } else{
        begin.ptr_ = end_;
    }
    return begin;
}

template<typename T, typename Allocator>
typename list<T, Allocator>::Iter list<T, Allocator>::end() const {
    Iter end;
    end.ptr_ = end_;
    return end;
}

template<typename T, typename Allocator>
list<T, Allocator>::~list() {
    Border_traits::destroy(border_alloc, reinterpret_cast<Border *>(rend_));
    Border_traits::deallocate(border_alloc, reinterpret_cast<Border *>(rend_), 1);
    Border_traits::destroy(border_alloc, reinterpret_cast<Border *>(end_));
    Border_traits::deallocate(border_alloc, reinterpret_cast<Border *>(end_), 1);
    if(tail_){
        tail_->next_ = nullptr;
    }
    for (Node *pv = head_; pv;) {
        Node *next = pv->next_;
        Node_traits::destroy(node_alloc_, pv);
        Node_traits::deallocate(node_alloc_, pv, 1);
        pv = next;
    }
}

template<typename T, typename Allocator>
typename list<T, Allocator>::Iter list<T, Allocator>::insert(const list::Iter &iter, T &&data) {
    Node *ptr = Node_traits::allocate(node_alloc_, 1);
    Node_traits::construct(node_alloc_, ptr, data);
    if (size_ == 0) {
        tail_ = head_ = ptr;
    }
    if (iter.ptr_ == head_) {
        head_ = ptr;
    }
    Node *prev = iter.ptr_->prev_;
    prev->next_ = ptr;
    ptr->prev_ = prev;
    ptr->next_ = iter.ptr_;
    iter.ptr_->prev_ = ptr;
    Iter iter_to_return;
    iter_to_return.ptr_ = ptr;
    ++size_;
    return iter_to_return;
}

template<typename T, typename Allocator>
typename list<T, Allocator>::Iter list<T, Allocator>::erase(const list::Iter &iter) {
    Iter iter_to_return;
    if (iter.ptr_ == end_|| iter.ptr_ == rend_){
        iter_to_return.ptr_ = end_;
    } else{
        iter.ptr_->next_->prev_ = iter.ptr_->prev_;
        iter.ptr_->prev_->next = iter.ptr_->next_;
    }
    Node_traits::destroy(node_alloc_, iter.ptr_);
    Node_traits::deallocate(node_alloc_, iter.ptr_, 1);
    return iter_to_return;
}

template<typename T, typename Allocator>
T list<T, Allocator>::front() {
    return head_->data_;
}

template<typename T, typename Allocator>
T list<T, Allocator>::back() {
    return tail_->data_;
}

//template<typename T, typename Allocator>
//void list<T, Allocator>::clear() {
//    if(tail_){
//        tail_->next_ = nullptr;
//    }
//    for (Node *pv = head_; pv;) {
//        Node *next = pv->next_;
//        Node_traits::destroy(node_alloc_, pv);
//        Node_traits::deallocate(node_alloc_, pv, 1);
//        pv = next;
//    }
//    head_ = tail_ = nullptr;
//    rend_->next_ = end_;
//    end_->prev_ = rend_;
//    size_ = 0;
//
//
//
//}

#endif //ARCHRONIS_LIST_HPP