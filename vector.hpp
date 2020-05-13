//
// Created by artemiy on 12.05.2020.
//

#ifndef ARCHRONIS_VECTOR_HPP
#define ARCHRONIS_VECTOR_HPP

#include <bits/allocator.h>
#include <memory>

template<typename T, typename Allocator = std::allocator<T>>
class vector{
private:
    using Alloc_traits = std::allocator_traits<Allocator>;
    Allocator alloc_;
    unsigned size_ = 0;
    unsigned capacity_ = 0;
    T* array_begin_ = nullptr;


public:
    vector() = default;
    vector(const unsigned size, const T&data, Allocator = Allocator());
    vector(const  unsigned size, Allocator = Allocator());
    ~vector();
    T&operator[](const unsigned index);
    const T& operator[](const unsigned index) const;
    unsigned size() const noexcept ;

};

template<typename T, typename Allocator>
vector<T, Allocator>::vector(const unsigned int size, const T &data, Allocator alloc): alloc_(alloc) {
    capacity_ = size_ = size;
    array_begin_ = Alloc_traits::allocate(alloc_, size);
    for(unsigned i = 0; i < size; ++i){
        Alloc_traits::construct(alloc_, array_begin_ + i, data);
    }
}

template<typename T, typename Allocator>
vector<T, Allocator>::vector(const unsigned int size, Allocator alloc) : alloc_(alloc) {
    capacity_ = size_ = size;
    array_begin_ = Alloc_traits::allocate(alloc_, size);
    for(unsigned i = 0; i < size; ++i){
        Alloc_traits::construct(alloc_, array_begin_ + i);
    }
}

template<typename T, typename Allocator>
vector<T, Allocator>::~vector()  {
    for(unsigned i = 0; i < size_; ++i){
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



#endif //ARCHRONIS_VECTOR_HPP
