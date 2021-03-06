//
// Created by artemiy on 14.0256.2020.
//

#ifndef ARCHRONIS_LZW_HPP
#define ARCHRONIS_LZW_HPP

#include <bits/allocator.h>
#include "memory"
#include <list>
#include <vector>
#include <stack>

//template<typename Allocator = std::allocator<char>>
class LZW_tree {
protected:
    unsigned size_ = 255;

    struct Node {

        u_char symbol_;
        u_int16_t value_;//max - 2^12?
        std::vector<std::shared_ptr<Node>> next_ = std::vector<std::shared_ptr<Node>>(256, nullptr);
        std::weak_ptr<Node> parent_;

        Node() = default;

        Node(const char symbol, const u_int16_t value, const std::weak_ptr<Node> &parent) :
                symbol_(symbol),
                value_(value),
                parent_(parent) {}
    };

    std::shared_ptr<Node> root;
    std::weak_ptr<Node> ptr_;
    std::string str;

    std::vector<std::weak_ptr<Node>> node_ptr_ = std::vector<std::weak_ptr<Node>>(4096);// сместить в декодер

    LZW_tree() :
            root(std::make_shared<Node>()),
            ptr_(root) {
        for (int i = 0; i < 256; ++i) {
            root->next_[i] = std::make_shared<Node>();// create Node
            root->next_[i]->symbol_ = root->next_[i]->value_ = i;// init fields
            root->next_[i]->parent_ = root;
            node_ptr_[i] = root->next_[i];// make link for O(1) access
        }
    }

    void resize() {
        size_ = 255;
        // удалить всех детей корня
        for (int i = 0; i < 256; ++i) {
            root->next_[i].reset();
        }
    }

    u_char get_digit_capacity() const {// - мб тут косяк

        unsigned cnt = 0;
        for (unsigned copy = size_; copy != 0; copy >>= 1, ++cnt);
        //return cnt;
        return cnt < 8 ? 8 : cnt;
    }
};

class Encoding_LZW_Tree : public LZW_tree {
private:
    unsigned prev_prev_size = size_;
    unsigned prev_size = size_;

    uint16_t output_code;
public:
    bool insert(const u_char symbol);

    void flush();

    uint16_t bits_in_next_code();

    unsigned return_code();

    Encoding_LZW_Tree() = default;
};

/**********************************************************************************************************************/
//проблема в том, что в алгоритме сначала идёт вывод, потом добавление. тут - наоборот:(
//решение - 100 - 107 строки вывести в отдельную функцию и вызывать её после вывода???
bool Encoding_LZW_Tree::insert(const u_char symbol) {
    if (ptr_.lock()->next_[symbol] != nullptr) {// если можем пойти дальше по дереву - идём
        ptr_ = ptr_.lock()->next_[symbol];
        return false;
    }
    //вставка символа
    //output_code = ptr_.lock()->value_;// Не должна ли она стоять ниже? - НЕТ // после коммита про ошибку - переезжает на одну строку вверх // СЮДА
    //prev_prev_size = size_;// на данном этапе это аналогично строке 96
    prev_prev_size = prev_size;
    prev_size = size_;

    //std::cout << static_cast<int>(symbol) << std::endl;

    ptr_.lock()->next_[symbol] = std::make_shared<Node>(symbol, ++size_, ptr_);
    // иначе - вставить символ на которыом оборвались и перейти к нему в корне

    output_code = ptr_.lock()->value_;// Не должна ли она стоять ниже? - НЕТ // после коммита про ошибку - переезжает на одну строку вверх
    //перевели указатель на еужную вершину
    ptr_ = root->next_[symbol];
    //output_code = ptr_.lock()->value_;

    return true;
    // дописать символ на котором прервались в эту цепочку(в дереве) - готово
    // перевести указатель на вершину, в которой прервались(в корне) - готово
}

void Encoding_LZW_Tree::flush() {
    prev_prev_size = prev_size;
    output_code = ptr_.lock()->value_;
}

//call when insert returned true
unsigned Encoding_LZW_Tree::return_code() {//return symbol code;
    //std::string copy = std::move(str);
    //str.clear();
    int copy = output_code;
    output_code = 0;
    return copy;
}

uint16_t Encoding_LZW_Tree::bits_in_next_code() {// возвращает не то что надо
    // должно вернуть - количество бит в output
    uint16_t cnt = 0;
    //for (unsigned copy = output_code; copy != 0; copy >>= 1, ++cnt);
    for (unsigned copy = prev_prev_size; copy != 0; copy >>= 1, ++cnt);
    //prev_prev_size = size_;// вот тут он снова присваивает не то
    return cnt < 8 ? 8 : cnt;
    //return get_digit_capacity();
}

/**********************************************************************************************************************/

class Decoding_LZW_Tree : public LZW_tree {
public:
    void check_code(u_int16_t code);

    std::string string_matches_code();

    uint16_t request_bits();
};

void Decoding_LZW_Tree::check_code(u_int16_t code) {


    std::weak_ptr<Node> pv = node_ptr_[code];

    //std::cout << "code : " << code << " symbol : " << static_cast<int>(pv.lock()->symbol_) << std::endl;

/*    if (pv.expired()) {
        ptr_.lock()->next_[ptr_.lock()->symbol_] = std::make_shared<Node>(ptr_.lock()->symbol_, size_ + 1, ptr_);
        pv = node_ptr_[code] = ptr_.lock()->next_[ptr_.lock()->symbol_];
        ++size_;


    } else*/ if (ptr_.lock()->next_[pv.lock()->symbol_] == nullptr) {
        // если не можем по нему перейти, то продолжим цепочку этим символом
        std::weak_ptr<Node> first_chain_symbol = node_ptr_[code];

        while (first_chain_symbol.lock()->parent_.lock() != root) {// нашли первый симфол цепочки.
            first_chain_symbol = first_chain_symbol.lock()->parent_;
        }
        ptr_.lock()->next_[first_chain_symbol.lock()->symbol_] =
                std::make_shared<Node>(first_chain_symbol.lock()->symbol_, size_ + 1, ptr_);

        // возможно ошибка ВЫШЕ НА СТРОКУ

        node_ptr_[size_ + 1] = ptr_.lock()->next_[first_chain_symbol.lock()->symbol_];
        ++size_;
    }

    ptr_ = pv.lock();// сместили указатель на правильную позицию. НУЖЕН ЛИ lock? В конечном счёте там node_ptr_[code]

    std::stack<char> st;
    while (pv.lock() != root) {// нашли первый симфол цепочки.
        st.push(pv.lock()->symbol_);
        pv = pv.lock()->parent_;
    }

    while (!st.empty()) {
        str.push_back(st.top());
        st.pop();
    }
/*
    if (pv.lock()->parent_.lock() ==
        ptr_.lock()) {//если символ с данным кодом - продолжение цепочки - ОШИБКА - не символ с данным кодом, а первый символ цепочки символа с данным кодом
        // проверять надо не если вспомогательный указатель - родитель символа с данным кодом, а если мы из можем из вспомогательного прийи в символ с данным кодом
        str.push_back(pv.lock()->symbol_);// записали значение
        ptr_ = pv;//сместились вниз.
        return false;
    }

    while (pv.lock()->parent_.lock() != root) {
        pv = pv.lock()->parent_;
    }

    ptr_.lock()->next_[pv.lock()->symbol_] = std::make_shared<Node>(pv.lock()->symbol_, size_++, ptr_);// НЕ ТО VALUE


    //ptr_ = root->next_[pv.lock()->symbol_];//какой из них
    ptr_ = node_ptr_[code];// правильный?? - ЭТОТ


    //прийти в начало цепочки с заданным кодом, чтобы узнать её первый символ + готово
    // и записать его в конец текущей цепочки.+ готово
    //перейти в записанный символ
    return true;
*/
}

std::string Decoding_LZW_Tree::string_matches_code() {
    std::string copy = std::move(str);
    str.clear();
    return copy;
/*    считать следуюхий код.
     если можно перейти дальше.
    иначе - вывести всю ветку + дополнить её + перейти по последнему коду

     на вершине в которой прервались и достроили - прийти в нее в корнеи топать дальше
*/
}

uint16_t Decoding_LZW_Tree::request_bits() {
    return get_digit_capacity();
}


#endif //ARCHRONIS_LZW_HPP
