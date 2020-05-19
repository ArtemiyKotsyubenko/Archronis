//
// Created by artemiy on 11.05.2020.
//

#ifndef ARCHRONIS_MEMORY_HPP
#define ARCHRONIS_MEMORY_HPP

#include <bits/move.h>

template<typename T>
struct default_deleter;

template<typename T, class Deleter = default_deleter<T>>
class shared_ptr;

template<typename T, class Deleter = default_deleter<T>>
class weak_ptr;

struct control_block {
    unsigned ptr_cnt = 0;
};

template<typename T, class Deleter>
class shared_ptr {
public:

    friend weak_ptr<T>;

    template<typename U, typename...Args>
    friend shared_ptr<U> make_shared(Args &&...args);

    shared_ptr() = default;

    shared_ptr(T *ptr);

    shared_ptr(const shared_ptr &other);

    shared_ptr(shared_ptr &&other) noexcept;

    shared_ptr<T> &operator=(const shared_ptr &other);

    shared_ptr<T> &operator=(shared_ptr &&other) noexcept;

    ~shared_ptr();

    bool operator==(const shared_ptr &other) const;

    bool operator!=(const shared_ptr &other) const;

    bool operator()() const;

    T& operator*();
    T* operator->();

    int use_count() const;

private:
    T *ptr_ = nullptr;
    control_block *control_block_;


};


template<typename T, class Deleter>
class weak_ptr {
public:
    weak_ptr() = default;

    weak_ptr(const weak_ptr &);

    explicit weak_ptr(const shared_ptr<T> &other);

    weak_ptr(weak_ptr &&other) noexcept;

    weak_ptr<T>& operator=(const shared_ptr<T>& other);

    weak_ptr &operator=(const weak_ptr &other) = default;

    weak_ptr<T> &operator=(weak_ptr &&other) noexcept;

    ~weak_ptr() = default;


    bool operator==(const weak_ptr &other) const;

    bool operator!=(const weak_ptr &other) const;

    shared_ptr<T> lock() const ;

    bool expired() const;

private:
    T *ptr_;
    control_block *control_block_;
};

//***************WEAK_PTR***********************

template<typename T, class Deleter>
weak_ptr<T, Deleter>::weak_ptr(const weak_ptr &other) {
    ptr_ = other.ptr_;
}

template<typename T, class Deleter>
weak_ptr<T, Deleter>::weak_ptr(const shared_ptr<T> &other) {
    ptr_ = other.ptr_;
    control_block_ = other.control_block_;
}

template<typename T, class Deleter>
bool weak_ptr<T, Deleter>::operator==(const weak_ptr &other) const {
    return ptr_ = other.ptr_;
}

template<typename T, class Deleter>
bool weak_ptr<T, Deleter>::operator!=(const weak_ptr &other) const {
    return ptr_ != other.ptr_;
}

template<typename T, class Deleter>
shared_ptr<T> weak_ptr<T, Deleter>::lock() const {
    shared_ptr<T> new_shared_ptr;
    new_shared_ptr.ptr_ = ptr_;
    new_shared_ptr.control_block_ = control_block_;
    return new_shared_ptr;
}

template<typename T, class Deleter>
bool weak_ptr<T, Deleter>::expired() const {
    return control_block_->ptr_cnt == 0;
}

template<typename T, class Deleter>
weak_ptr<T, Deleter>::weak_ptr(weak_ptr &&other) noexcept {
    ptr_ = other.ptr_;
    control_block_ = other.control_block_;
    other.control_block_ = other.ptr_ = nullptr;
}

template<typename T, class Deleter>
weak_ptr<T> &weak_ptr<T, Deleter>::operator=(weak_ptr &&other) noexcept {
    ptr_ = other.ptr_;
    control_block_ = other.control_block_;
    other.control_block_ = other.ptr_ = nullptr;
    return *this;
}

template<typename T, class Deleter>
weak_ptr<T> &weak_ptr<T, Deleter>::operator=(const shared_ptr<T> &other) {
    ptr_ = other.ptr_;
    control_block_ = other.control_block_;
    return ;
}


//***************WEAK_PTR***********************


//***************SHARED_PTR*********************

template<typename T, typename...Args>
shared_ptr<T> make_shared(Args &&...args) {
    shared_ptr<T> smart_ptr;
//    char *allocated_memory = new char[sizeof(T) + sizeof(shared_ptr<T>::control_block)];
//    smart_ptr.ptr = new(allocated_memory) T(std::forward<Args>(args)...);
//    smart_ptr.control_block_ = new(allocated_memory + sizeof(T)) control_block();

    smart_ptr.ptr_ = new T(std::forward<Args>(args)...);
    smart_ptr.control_block_ = new control_block();
    return smart_ptr;
}

template<typename T, class Deleter>
shared_ptr<T, Deleter>::shared_ptr(T *ptr) {
    ptr_ = ptr;
    control_block_ = new control_block;
}

template<typename T, class Deleter>
shared_ptr<T, Deleter>::shared_ptr(const shared_ptr &other) {
    ptr_ = other.ptr_;
    control_block_ = other.control_block_;
    ++control_block_->ptr_cnt;
}

template<typename T, class Deleter>
shared_ptr<T> &shared_ptr<T, Deleter>::operator=(const shared_ptr &other) {
    if (*this != other) {
        --control_block_->ptr_cnt;

        control_block_->ptr_cnt = other.control_block_->ptr_cnt;
        ptr_ = other.ptr_;
        ++control_block_->ptr_cnt;
    }
    return *this;
}

template<typename T, class Deleter>
shared_ptr<T, Deleter>::~shared_ptr() {
    --control_block_->ptr_cnt;
    if (control_block_->ptr_cnt == 0) {
        delete control_block_;
        delete ptr_;
    }
}

template<typename T, class Deleter>
bool shared_ptr<T, Deleter>::operator==(const shared_ptr &other) const {
    return ptr_ == other.ptr_;
}

template<typename T, class Deleter>
bool shared_ptr<T, Deleter>::operator!=(const shared_ptr &other) const {
    return ptr_ != other.ptr_;
}

template<typename T, class Deleter>
bool shared_ptr<T, Deleter>::operator()() const {
    return ptr_ != nullptr;
}

template<typename T, class Deleter>
int shared_ptr<T, Deleter>::use_count() const {
    return control_block_->ptr_cnt;
}

template<typename T, class Deleter>
T& shared_ptr<T, Deleter>::operator*() {
    return *ptr_;
}

template<typename T, class Deleter>
shared_ptr<T, Deleter>::shared_ptr(shared_ptr &&other) noexcept {
    ptr_ = other.ptr_;
    control_block_ = other.control_block_;
    other.control_block_ = other.ptr_ = nullptr;
}

template<typename T, class Deleter>
shared_ptr<T> &shared_ptr<T, Deleter>::operator=(shared_ptr &&other) noexcept {
    ptr_ = other.ptr_;
    control_block_ = other.control_block_;
    other.control_block_ = other.ptr_ = nullptr;
}

template<typename T, class Deleter>
T *shared_ptr<T, Deleter>::operator->() {
    return ptr_;
}

//***************SHARED_PTR*********************


template<typename T>
struct default_deleter {
    void operator()(T *ptr) {
        delete ptr;
    }
};

template<typename T>
struct default_deleter<T[]> {
    void operator()(T *ptr) {
        delete[] ptr;
    }
};

#endif //ARCHRONIS_MEMORY_HPP
