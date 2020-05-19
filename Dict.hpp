//
// Created by artemiy on 18.05.2020.
//

#ifndef ARCHRONIS_DICT_HPP
#define ARCHRONIS_DICT_HPP

#include "list.hpp"
#include "vector.hpp"
#include "String.hpp"

template<typename T>
struct hasher {

    unsigned operator()(const T& key);
};

template<>
struct hasher<string> {
private:
    const unsigned seed = 1283;
public:

    unsigned operator()(const string& str) const {
        unsigned hash = 0;

        for (auto ch : str) {
            //hash = (hash + ch)* seed;
            (hash += ch) *= seed; // <=>  hash = (hash + ch)* seed;
        }
        return hash;
    }
};


template<>
struct hasher<uint16_t> {
public:
    unsigned operator()(const uint16_t i) const {
        return i;
    }
};


template<typename Key, typename Value, class Hasher = hasher<Key>>
class dict {
private:
    Hasher hasher;

    vector<list<std::pair < Key, Value>>>
    dct;
    unsigned size_ = 0;
    unsigned capacity;

public:
    explicit dict(const unsigned size) :
            dct(size),
            capacity(size) {}

    void* find(const Key& key) {
        int hash = hasher(key);
        hash %= capacity;
        auto& chain = dct[hash];
        for (const auto& it: chain) {
            if (it.first == key) {
                return &hasher;
            }
        }
        return nullptr;
    }

    Value operator[](const Key& key) const {
        unsigned hash = hasher(key);
        hash %= capacity;
        auto& chain = dct[hash];
        for (auto& it: chain) {
            if (it.first == key) {
                return it.second;
            }
        }
        return chain.end()->second;
    }

    void* end() {
        return nullptr;
    }

    void insert(std::pair <Key, Value> pair) {
        unsigned hash = hasher(pair.first);
        hash %= capacity;
        dct[hash].push_back(pair);
        ++size_;
    }

    unsigned size() const {
        return size_;
    }

};


#endif //ARCHRONIS_DICT_HPP
