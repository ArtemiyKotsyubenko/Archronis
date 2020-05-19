//
// Created by artemiy on 12.05.2020.
//

#ifndef ARCHRONIS_VECTOR_HPP
#define ARCHRONIS_VECTOR_HPP

#include <bits/allocator.h>
#include <memory>

template<typename T, typename Allocator = std::allocator<T>>
class vector {
private:
    using Alloc_traits = std::allocator_traits<Allocator>;
    Allocator alloc_;
    unsigned size_ = 0;
    unsigned capacity_ = 0;
    T *array_begin_ = nullptr;


public:
    vector() = default;

    vector(const unsigned size, const T &data, Allocator = Allocator());

    vector(const unsigned size, Allocator = Allocator());

    vector(const vector &other);

    vector(vector &&other) /*noexcept(noexcept(std::declval<T>().T(std::move(T())))))*/;

    ~vector();

    struct Iter;
    using iterator = Iter;

    T &operator[](const unsigned index);

    const T &operator[](const unsigned index) const;

    unsigned size() const noexcept;

    unsigned capacity() const noexcept;

    void push_back(const T &);

    void push_back(T &&);

    template<typename ...Args>
    void emplace_back(Args &&...args);

    void pop_back();

    void resize(const unsigned new_size);

    void reserve(const unsigned new_capacity);

    void clear();

    Iter begin() const;

    Iter end() const;

};

template<typename T, typename Allocator>
struct vector<T, Allocator>::Iter {
public:
    friend vector;

    T &operator*();

    const T &operator*() const;

    T *operator->();

    const T *operator->() const;

    Iter &operator++();

    const Iter operator++(int);

    Iter &operator--();

    const Iter operator--(int);


    Iter(const Iter &other);

    Iter(Iter &&other) noexcept;

    Iter() = default;

    Iter &operator=(const Iter &other);

    Iter &operator=(Iter &&other) noexcept;

    int operator-(const Iter &other);

    Iter operator+(const unsigned i);

    Iter operator-(const unsigned i);

    Iter &operator-=(const unsigned i);

    Iter &operator+=(const unsigned i);

    bool operator==(const Iter &other) const;

    bool operator!=(const Iter &other) const;

private:
    mutable T *ptr_ = nullptr;
};

template<typename T, typename Allocator>
T &vector<T, Allocator>::Iter::operator*() {
    return *ptr_;
}

template<typename T, typename Allocator>
const T &vector<T, Allocator>::Iter::operator*() const {
    return *ptr_;
}

template<typename T, typename Allocator>
T *vector<T, Allocator>::Iter::operator->() {
    return ptr_;
}

template<typename T, typename Allocator>
const T *vector<T, Allocator>::Iter::operator->() const {
    return ptr_;
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::Iter &vector<T, Allocator>::Iter::operator++() {
    ++ptr_;
    return *this;
}

template<typename T, typename Allocator>
const typename vector<T, Allocator>::Iter vector<T, Allocator>::Iter::operator++(int) {
    Iter copy = *this;
    ++ptr_;
    return copy;
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::Iter &vector<T, Allocator>::Iter::operator--() {
    --ptr_;
    return *this;
}

template<typename T, typename Allocator>
const typename vector<T, Allocator>::Iter vector<T, Allocator>::Iter::operator--(int) {
    Iter copy = *this;
    --ptr_;
    return copy;
}

template<typename T, typename Allocator>
vector<T, Allocator>::Iter::Iter(const Iter &other):
        ptr_(other.ptr_) {}

template<typename T, typename Allocator>
vector<T, Allocator>::Iter::Iter(Iter &&other) noexcept:
        ptr_(other.ptr_) {
    other.ptr_ = nullptr;
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::Iter &vector<T, Allocator>::Iter::operator=(const Iter &other) {
    ptr_ = other.ptr_;
    return *this;
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::Iter &vector<T, Allocator>::Iter::operator=(Iter &&other) noexcept {
    ptr_ = other.ptr_;
    other.ptr_ = nullptr;
    return *this;
}

template<typename T, typename Allocator>
int vector<T, Allocator>::Iter::operator-(const Iter &other) {
    return ptr_ - other.ptr_;
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::Iter& vector<T, Allocator>::Iter::operator-=(const unsigned int i) {
    ptr_-= i;
    return *this;
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::Iter &vector<T, Allocator>::Iter::operator+=(const unsigned int i) {
    ptr_+= i;
    return *this;
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::Iter vector<T, Allocator>::Iter::operator+(const unsigned int i) {
    Iter it;
    it.ptr_ = ptr_ + i;
    return it;
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::Iter vector<T, Allocator>::Iter::operator-(const unsigned int i) {
    Iter it;
    it.ptr_ = ptr_ - i;
    return it;
}

template<typename T, typename Allocator>
bool vector<T, Allocator>::Iter::operator==(const vector<T, Allocator>::Iter &other) const {
    return ptr_ == other.ptr_;
}

template<typename T, typename Allocator>
bool vector<T, Allocator>::Iter::operator!=(const vector<T, Allocator>::Iter &other) const {
    return ptr_ != other.ptr_;
}


template<typename T, typename Allocator>
vector<T, Allocator>::vector(const unsigned int size, const T &data, Allocator alloc): alloc_(alloc) {
    capacity_ = size_ = size;
    array_begin_ = Alloc_traits::allocate(alloc_, size);
    for (unsigned i = 0; i < size; ++i) {
        Alloc_traits::construct(alloc_, array_begin_ + i, data);
    }
}

template<typename T, typename Allocator>
vector<T, Allocator>::vector(const unsigned int size, Allocator alloc) : alloc_(alloc) {
    capacity_ = size_ = size;
    array_begin_ = Alloc_traits::allocate(alloc_, size);
    for (unsigned i = 0; i < size; ++i) {
        Alloc_traits::construct(alloc_, array_begin_ + i);
    }
}

template<typename T, typename Allocator>
vector<T, Allocator>::~vector() {
    for (unsigned i = 0; i < size_; ++i) {
        Alloc_traits::destroy(alloc_, array_begin_ + i);
    }
    Alloc_traits::deallocate(alloc_, array_begin_, size_);
}

template<typename T, typename Allocator>
T &vector<T, Allocator>::operator[](const unsigned int index) {
    return *(array_begin_ + index);
}

template<typename T, typename Allocator>
const T &vector<T, Allocator>::operator[](const unsigned int index) const {
    return *(array_begin_ + index);
}


template<typename T, typename Allocator>
unsigned vector<T, Allocator>::size() const noexcept {
    return size_;
}

template<typename T, typename Allocator>
unsigned vector<T, Allocator>::capacity() const noexcept {
    return capacity_;
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::Iter vector<T, Allocator>::begin()const {
    Iter it;
    it.ptr_ = array_begin_;
    return it;
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::Iter vector<T, Allocator>::end() const {
    Iter it;
    it.ptr_ = array_begin_ + size_;
    return it;
}

template<typename T, typename Allocator>
void vector<T, Allocator>::pop_back() {
    Alloc_traits::destroy(alloc_, array_begin_ + --size_);
}

template<typename T, typename Allocator>
void vector<T, Allocator>::push_back(const T &data) {
    if (size_ + 1 <= capacity_) {
        Alloc_traits::construct(alloc_, array_begin_ + size_, data);
        ++size_;
    } else {
        T *new_array_begin = Alloc_traits::allocate(alloc_, size_ * 2);
        Alloc_traits::construct(alloc_, new_array_begin + size_, data);

        for (unsigned i = 0; i < size_; ++i) {
            Alloc_traits::construct(alloc_, new_array_begin + i, std::move_if_noexcept(array_begin_[i]));
        }

        for (unsigned i = 0; i < size_; ++i) {
            Alloc_traits::destroy(alloc_, array_begin_ + i);
        }

        Alloc_traits::deallocate(alloc_, array_begin_, size_);
        array_begin_ = new_array_begin;
        capacity_ *= 2;
        ++size_;
    }
}

template<typename T, typename Allocator>
void vector<T, Allocator>::push_back(T &&data) {
    if (size_ + 1 <= capacity_) {
        Alloc_traits::construct(alloc_, array_begin_ + size_, data);
        ++size_;
    } else {
        T *new_array_begin = Alloc_traits::allocate(alloc_, size_ * 2);
        Alloc_traits::construct(alloc_, new_array_begin + size_, data);

        for (unsigned i = 0; i < size_; ++i) {
            Alloc_traits::construct(alloc_, new_array_begin + i, std::move_if_noexcept(array_begin_[i]));
        }

        for (unsigned i = 0; i < size_; ++i) {
            Alloc_traits::destroy(alloc_, array_begin_ + i);
        }

        Alloc_traits::deallocate(alloc_, array_begin_, size_);
        array_begin_ = new_array_begin;
        capacity_ *= 2;
        ++size_;
    }
}

template<typename T, typename Allocator>
template<typename... Args>
void vector<T, Allocator>::emplace_back(Args &&... args) {
    if (size_ + 1 <= capacity_) {
        Alloc_traits::construct(alloc_, array_begin_ + size_, std::forward<Args>(args)...);
        ++size_;
    } else {
        T *new_array_begin = Alloc_traits::allocate(alloc_, size_ * 2);
        Alloc_traits::construct(alloc_, new_array_begin + size_, std::forward<Args>(args)...);

        for (unsigned i = 0; i < size_; ++i) {
            Alloc_traits::construct(alloc_, new_array_begin + i, std::move_if_noexcept(array_begin_[i]));
        }

        for (unsigned i = 0; i < size_; ++i) {
            Alloc_traits::destroy(alloc_, array_begin_ + i);
        }

        Alloc_traits::deallocate(alloc_, array_begin_, size_);
        array_begin_ = new_array_begin;
        capacity_ *= 2;
        ++size_;
    }
}

template<typename T, typename Allocator>
vector<T, Allocator>::vector(const vector &other) {
    capacity_ = other.capacity_;
    size_ = other.size_;
    array_begin_ = Alloc_traits::allocate(alloc_, capacity_);

    for (unsigned i = 0; i < size_; ++i) {
        Alloc_traits :construct(alloc_, array_begin_ + i, other[i]);
    }
}

template<typename T, typename Allocator>
vector<T, Allocator>::vector(vector &&other){
    capacity_ = other.capacity_;
    size_ = other.size_;
    array_begin_ = Alloc_traits::allocate(alloc_, capacity_);

    for (unsigned i = 0; i < size_; ++i) {
        Alloc_traits ::construct(alloc_, array_begin_ + i, std::move(other[i]));
    }

    Alloc_traits::deallocate(other.alloc_, other.array_begin_, capacity_);
    other.array_begin_ = nullptr;
}

template<typename T, typename Allocator>
void vector<T, Allocator>::resize(const unsigned int new_size) {
    if(new_size > size_){
        if(size_ <= capacity_){
            for(unsigned i = size_; i < new_size; ++i){
                Alloc_traits::construct(alloc_, array_begin_ + i);
            }
            size_ = new_size;
        } else{

            T *new_array_begin = Alloc_traits::allocate(alloc_, capacity_ * 2);
            for(unsigned i = size_; i < new_size; ++i){
                Alloc_traits::construct(alloc_, new_array_begin + i);
            }

            for (unsigned i = 0; i < size_; ++i) {
                Alloc_traits::construct(alloc_, new_array_begin + i, std::move_if_noexcept(array_begin_[i]));
            }

            for (unsigned i = 0; i < size_; ++i) {
                Alloc_traits::destroy(alloc_, array_begin_ + i);
            }

            Alloc_traits::deallocate(alloc_, array_begin_, size_);
            array_begin_ = new_array_begin;
            capacity_ *= 2;
            size_ = new_size;
        }
    }

}

template<typename T, typename Allocator>
void vector<T, Allocator>::clear() {
    for (unsigned  i = 0; i < size_; ++i){
        Alloc_traits::destroy(alloc_, array_begin_+ i);
    }
    size_ = 0;
}

template<typename T, typename Allocator>
void vector<T, Allocator>::reserve(const unsigned int new_capacity) {
    if(new_capacity > capacity_){
        T *new_array_begin = Alloc_traits::allocate(alloc_, new_capacity);

        for (unsigned i = 0; i < size_; ++i) {
            Alloc_traits::construct(alloc_, new_array_begin + i, std::move_if_noexcept(array_begin_[i]));
        }

        for (unsigned i = 0; i < size_; ++i) {
            Alloc_traits::destroy(alloc_, array_begin_ + i);
        }

        Alloc_traits::deallocate(alloc_, array_begin_, size_);
        array_begin_ = new_array_begin;
        capacity_ = new_capacity;
    }

}


#endif //ARCHRONIS_VECTOR_HPP
