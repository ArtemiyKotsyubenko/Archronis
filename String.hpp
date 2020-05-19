//
// Created by artemiy on 18.05.2020.
//

#ifndef ARCHRONIS_STRING_HPP
#define ARCHRONIS_STRING_HPP

#include <bits/allocator.h>
#include <memory>

template<typename Allocator = std::allocator<char>>
class String {

private:
    using Alloc_traits = std::allocator_traits<Allocator>;
    Allocator alloc_;
    unsigned size_ = 0;
    unsigned capacity_ = 0;
    char *array_begin_ = nullptr;

public:
    String() = default;

    String(const unsigned size, const char &data, Allocator = Allocator());

    String(const unsigned size, Allocator = Allocator());

    String(const String &other);

    String(String &&other) /*noexcept(noexcept(std::declval<T>().T(std::move(T())))))*/;

    ~String();

    struct Iter;
    using iterator = Iter;

    char &operator[](const unsigned index);

    const char &operator[](const unsigned index) const;

    unsigned size() const noexcept;

    unsigned capacity() const noexcept;

    void push_back(const char &);

    void push_back(char &&);

    template<typename ...Args>
    void emplace_back(Args &&...args);

    void pop_back();

    void resize(const unsigned new_size);

    void reserve(const unsigned new_capacity);

    void clear();

    Iter begin() const;

    Iter end() const;

    bool operator==(const String &other) const noexcept;

    String &operator+=(const String &other);

    String operator+(const String &other);

    String &operator=(const String &other);

};

using string = String<std::allocator<char>>;

template<typename Allocator>
struct String<Allocator>::Iter {
public:
    friend String;

    char &operator*();

    const char &operator*() const;

    char *operator->();

    const char *operator->() const;

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
    mutable char *ptr_ = nullptr;
};

template<typename Allocator>
char &String<Allocator>::Iter::operator*() {
    return *ptr_;
}

template<typename Allocator>
const char &String<Allocator>::Iter::operator*() const {
    return *ptr_;
}

template<typename Allocator>
char *String<Allocator>::Iter::operator->() {
    return ptr_;
}

template<typename Allocator>
const char *String<Allocator>::Iter::operator->() const {
    return ptr_;
}

template<typename Allocator>
typename String<Allocator>::Iter &String<Allocator>::Iter::operator++() {
    ++ptr_;
    return *this;
}

template<typename Allocator>
const typename String<Allocator>::Iter String<Allocator>::Iter::operator++(int) {
    Iter copy = *this;
    ++ptr_;
    return copy;
}

template<typename Allocator>
typename String<Allocator>::Iter &String<Allocator>::Iter::operator--() {
    --ptr_;
    return *this;
}

template<typename Allocator>
const typename String<Allocator>::Iter String<Allocator>::Iter::operator--(int) {
    Iter copy = *this;
    --ptr_;
    return copy;
}

template<typename Allocator>
String<Allocator>::Iter::Iter(const Iter &other):
        ptr_(other.ptr_) {}

template<typename Allocator>
String<Allocator>::Iter::Iter(Iter &&other) noexcept:
        ptr_(other.ptr_) {
    other.ptr_ = nullptr;
}

template<typename Allocator>
typename String<Allocator>::Iter &String<Allocator>::Iter::operator=(const Iter &other) {
    ptr_ = other.ptr_;
    return *this;
}

template<typename Allocator>
typename String<Allocator>::Iter &String<Allocator>::Iter::operator=(Iter &&other) noexcept {
    ptr_ = other.ptr_;
    other.ptr_ = nullptr;
    return *this;
}

template<typename Allocator>
int String<Allocator>::Iter::operator-(const Iter &other) {
    return ptr_ - other.ptr_;
}

template<typename Allocator>
typename String<Allocator>::Iter &String<Allocator>::Iter::operator-=(const unsigned int i) {
    ptr_ -= i;
    return *this;
}

template<typename Allocator>
typename String<Allocator>::Iter &String<Allocator>::Iter::operator+=(const unsigned int i) {
    ptr_ += i;
    return *this;
}

template<typename Allocator>
typename String<Allocator>::Iter String<Allocator>::Iter::operator+(const unsigned int i) {
    Iter it;
    it.ptr_ = ptr_ + i;
    return it;
}

template<typename Allocator>
typename String<Allocator>::Iter String<Allocator>::Iter::operator-(const unsigned int i) {
    Iter it;
    it.ptr_ = ptr_ - i;
    return it;
}

template<typename Allocator>
bool String<Allocator>::Iter::operator==(const String<Allocator>::Iter &other) const {
    return ptr_ == other.ptr_;
}

template<typename Allocator>
bool String<Allocator>::Iter::operator!=(const String<Allocator>::Iter &other) const {
    return ptr_ != other.ptr_;
}


template<typename Allocator>
String<Allocator>::String(const unsigned int size, const char &data, Allocator alloc): alloc_(alloc) {
    capacity_ = size_ = size;
    array_begin_ = Alloc_traits::allocate(alloc_, size);
    for (unsigned i = 0; i < size; ++i) {
        Alloc_traits::construct(alloc_, array_begin_ + i, data);
    }
}

template<typename Allocator>
String<Allocator>::String(const unsigned int size, Allocator alloc) : alloc_(alloc) {
    capacity_ = size_ = size;
    array_begin_ = Alloc_traits::allocate(alloc_, size);
    for (unsigned i = 0; i < size; ++i) {
        Alloc_traits::construct(alloc_, array_begin_ + i);
    }
}

template<typename Allocator>
String<Allocator>::~String() {
    for (unsigned i = 0; i < size_; ++i) {
        Alloc_traits::destroy(alloc_, array_begin_ + i);
    }
    Alloc_traits::deallocate(alloc_, array_begin_, size_);
}

template<typename Allocator>
char &String<Allocator>::operator[](const unsigned int index) {
    return *(array_begin_ + index);
}

template<typename Allocator>
const char &String<Allocator>::operator[](const unsigned int index) const {
    return *(array_begin_ + index);
}


template<typename Allocator>
unsigned String<Allocator>::size() const noexcept {
    return size_;
}

template<typename Allocator>
unsigned String<Allocator>::capacity() const noexcept {
    return capacity_;
}

template<typename Allocator>
typename String<Allocator>::Iter String<Allocator>::begin() const {
    Iter it;
    it.ptr_ = array_begin_;
    return it;
}

template<typename Allocator>
typename String<Allocator>::Iter String<Allocator>::end() const {
    Iter it;
    it.ptr_ = array_begin_ + size_;
    return it;
}

template<typename Allocator>
void String<Allocator>::pop_back() {
    Alloc_traits::destroy(alloc_, array_begin_ + --size_);
}

template<typename Allocator>
void String<Allocator>::push_back(const char &data) {
    if (size_ + 1 <= capacity_) {
        Alloc_traits::construct(alloc_, array_begin_ + size_, data);
        ++size_;
    } else {
        char *new_array_begin = Alloc_traits::allocate(alloc_, size_ * 2);
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

template<typename Allocator>
void String<Allocator>::push_back(char &&data) {
    if (size_ + 1 <= capacity_) {
        Alloc_traits::construct(alloc_, array_begin_ + size_, data);
        ++size_;
    } else {
        char *new_array_begin = Alloc_traits::allocate(alloc_, size_ * 2);
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

template<typename Allocator>
template<typename... Args>
void String<Allocator>::emplace_back(Args &&... args) {
    if (size_ + 1 <= capacity_) {
        Alloc_traits::construct(alloc_, array_begin_ + size_, std::forward<Args>(args)...);
        ++size_;
    } else {
        char *new_array_begin = Alloc_traits::allocate(alloc_, size_ * 2);
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

template<typename Allocator>
String<Allocator>::String(const String &other) {
    capacity_ = other.capacity_;
    size_ = other.size_;
    array_begin_ = Alloc_traits::allocate(alloc_, capacity_);

    for (unsigned i = 0; i < size_; ++i) {
        Alloc_traits::construct(alloc_, array_begin_ + i, other[i]);
    }
}

template<typename Allocator>
String<Allocator>::String(String &&other) {
    capacity_ = other.capacity_;
    size_ = other.size_;
    array_begin_ = Alloc_traits::allocate(alloc_, capacity_);

    for (unsigned i = 0; i < size_; ++i) {
        Alloc_traits::construct(alloc_, array_begin_ + i, std::move(other[i]));
    }

    Alloc_traits::deallocate(other.alloc_, other.array_begin_, capacity_);
    other.array_begin_ = nullptr;
}

template<typename Allocator>
void String<Allocator>::resize(const unsigned int new_size) {
    if (new_size > size_) {
        if (size_ <= capacity_) {
            for (unsigned i = size_; i < new_size; ++i) {
                Alloc_traits::construct(alloc_, array_begin_ + i);
            }
            size_ = new_size;
        } else {

            char *new_array_begin = Alloc_traits::allocate(alloc_, capacity_ * 2);
            for (unsigned i = size_; i < new_size; ++i) {
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

template<typename Allocator>
void String<Allocator>::clear() {
    for (unsigned i = 0; i < size_; ++i) {
        Alloc_traits::destroy(alloc_, array_begin_ + i);
    }
    size_ = 0;
}

template<typename Allocator>
void String<Allocator>::reserve(const unsigned int new_capacity) {
    if (new_capacity > capacity_) {
        char *new_array_begin = Alloc_traits::allocate(alloc_, new_capacity);

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

template<typename Allocator>
bool String<Allocator>::operator==(const String &other) const noexcept {
    if (size_ != other.size_) {
        return false;
    }
    for (int i = 0; i < size_; ++i) {
        if (array_begin_[i] != other[i]) {
            return false;
        }
    }
    return true;
}

template<typename Allocator>
String<Allocator> &String<Allocator>::operator+=(const String &other) {

    reserve(size_ + other.size_);


    for (int i = 0; i < other.size_; ++i) {
        array_begin_[size_ + i] = other[i];
    }

    size_ = size_ + other.size_;

    return *this;
}

template<typename Allocator>
String<Allocator> String<Allocator>::operator+(const String &other) {
    String str;
    str += *this;
    str += other;
    return str;
}

template<typename Allocator>
String<Allocator> &String<Allocator>::operator=(const String &other) {
    reserve(other.size_);
    for (int i = 0; i < other.size_; ++i) {
        array_begin_[i] = other[i];
    }

    size_ = other.size_;

    return *this;
}


#endif //ARCHRONIS_STRING_HPP
