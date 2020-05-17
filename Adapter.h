//
//  Adapter.h
//  LZW_a
//
//  Created by Alexey Ischenko on 17.05.2020.
//  Copyright © 2020 Enthropy. All rights reserved.
//

#ifndef Adapter_h
#define Adapter_h

#include <fstream>
#include <cstdint>
#include <cmath>

class AdapterOut {
private:
    int8_t curr_l = 0;
    std::ofstream& fout;
    int32_t buff = 0;
    int8_t curr_pos = 0;
    void push_to_stream();
public:
    AdapterOut(const int8_t& cl_, std::ofstream& fout_) : curr_l(cl_), fout(fout_) {}
    ~AdapterOut() = default;
    void change_cl(const int16_t& cl_) {curr_l = cl_;}
    void add_int(const int16_t& num);
};

class AdapterIn {
private:
    int8_t curr_l = 0;
    std::ifstream& fin;
    uint32_t buff = 0;
    int8_t curr_pos = 0;
    void get_from_stream();
public:
    AdapterIn(const int8_t& cl_, std::ifstream& fin_) : curr_l(cl_), fin(fin_) {}
    ~AdapterIn() = default;
    void change_cl(const int16_t& cl_) {curr_l = cl_;}
    int16_t get_int();

    void push_char(u_char temp);
    int8_t size() const {return curr_pos;}
    int16_t alt_get_int();
};

void AdapterIn::push_char(u_char temp) {
    buff <<= 8;
    curr_pos += 8;

    //fin >> temp;
    buff += temp;
}

int16_t AdapterIn::alt_get_int() {
    curr_pos -= curr_l;
    int16_t ans = buff >> (curr_pos);
    buff <<= 31 - curr_pos;
    buff >>= 31 - curr_pos;
    return ans;
}

void AdapterOut::add_int(const int16_t &num) {
    buff <<= curr_l;
    curr_pos += curr_l;
    buff += num;
    push_to_stream();
}

int16_t AdapterIn::get_int() {
    get_from_stream();
    curr_pos -= curr_l;
    int16_t ans = buff >> (curr_pos);
    buff <<= 31 - curr_pos;
    buff >>= 31 - curr_pos;
    return ans;
}

void AdapterOut::push_to_stream() {
    while (curr_pos >= curr_l) {
        curr_pos -= 8;
        char temp = buff >> (curr_pos);
        fout << temp;
        buff <<= 31 - curr_pos;
        buff >>= 31 - curr_pos;
    }
}

void AdapterIn::get_from_stream() {
    while (curr_pos < curr_l) {
        buff <<= 8;
        curr_pos += 8;
        char temp;
        fin >> temp;
        buff += temp;
    }
}

#endif /* Adapter_h */