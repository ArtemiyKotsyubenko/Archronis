//
// Created by artemiy on 18.05.2020.
//

#ifndef ARCHRONIS_CONTAINERADAPTER_HPP
#define ARCHRONIS_CONTAINERADAPTER_HPP
#include "list.hpp"

template<typename T,  class Container = list<T>>
class stack{
private:
    Container container;
public:

    stack() = default;

    ~stack() = default;

    void push(const T& data);

    void push(T&& data);

    template<typename ...Args>
    void emplace(Args&&...args);

    void pop();

    T top() const;

    unsigned size() const;

    bool empty() const ;
};

template<typename T, class Container>
void stack<T, Container>::push(const T& data) {
    container.push_back(data);
}

template<typename T, class Container>
void stack<T, Container>::push(T &&data) {
    container.push_back(std::move(data));
}

template<typename T, class Container>
template<typename... Args>
void stack<T, Container>::emplace(Args&&...args) {
    container.emplace_back(std::forward<Args>(args)...);
}

template<typename T, class Container>
void stack<T, Container>::pop() {
    container.pop_back();
}

template<typename T, class Container>
T stack<T, Container>::top() const {
    return container.back();
}

template<typename T, class Container>
unsigned stack<T, Container>::size() const {
    return container.size();
}

template<typename T, class Container>
bool stack<T, Container>::empty() const {
    return container.size();
}


template<typename T,  class Container = list<T>>
class queue{
private:
    Container container;
public:
    queue() = default;

    ~queue() = default;

    void push(const  T&data );

    void push(T&& data);

    template<typename ...Args>
    void emplace(Args&&...args);

    void pop();

    T front() const;

    T back() const;

    unsigned size() const;

    bool empty() const ;


};

template<typename T, class Container>
void queue<T, Container>::push(const  T&data) {
    container.push_back(data);
}

template<typename T, class Container>
void queue<T, Container>::push(T &&data) {
    container.push_back(std::move(data));
}

template<typename T, class Container>
template<typename... Args>
void queue<T, Container>::emplace(Args&&...args) {
    container.emplace_back(std::forward<Args>(args)...);
}

template<typename T, class Container>
void queue<T, Container>::pop() {
    container.pop_front();
}

template<typename T, class Container>
T queue<T, Container>::front() const {
    return container.front();
}

template<typename T, class Container>
T queue<T, Container>::back() const {
    return container.back();
}

template<typename T, class Container>
unsigned queue<T, Container>::size() const {
    return container.size();
}

template<typename T, class Container>
bool queue<T, Container>::empty() const {
    return container.size();
}

#endif //ARCHRONIS_CONTAINERADAPTER_HPP
