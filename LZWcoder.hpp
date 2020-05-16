//
// Created by artemiy on 15.05.2020.
//

#ifndef ARCHRONIS_LZWCODER_HPP
#define ARCHRONIS_LZWCODER_HPP

#include "Streams.h"
#include "LZW.hpp"

class CoderLZW {
private:

    Ifstream_wrap Fin;
    Ofstream_wrap Fout;

protected:

    struct counter {
    private:
        unsigned char i = 0;
        const unsigned char mod;
    public:

        const int operator++() {
            ++i %= mod;
            return i;
        }

        const int operator+=(const u_char num) {
            return (i += num) %= mod;
        }

        const int operator-=(const u_char num) {
            return (i -= num) %= mod;
        }

        operator u_char() const { return i; }

        operator u_int32_t const() { return i; }

        counter(const unsigned char mod_) : mod(mod_) {}
    };

    //u_char current_byte;
    u_char byte_to_write;
    std::ifstream &fin;
    std::ofstream &fout;

    CoderLZW(const char *input_file, const char *output_file) :
            Fin(input_file),
            Fout(output_file),
            fin(Fin.file),
            fout(Fout.file) {}

    ~CoderLZW() = default;

};

class EncoderLZW : protected CoderLZW {
private:
    counter cnt_ = counter(8);
    Encoding_LZW_Tree tree;
    uint16_t code_;

    //unsigned get_code(unsigned code);// преобразует переданное число в соответствующую числу бит битовую запись
public:
    EncoderLZW(const char *input_file, const char *output_file);

};

EncoderLZW::EncoderLZW(const char *input_file, const char *output_file) :
        CoderLZW(input_file, output_file) {

    if (fin.peek() != EOF) {
        for (char ch = 0; fin.get(ch);) {
            u_char current_byte = ch;
            if (tree.insert(current_byte)) {
                code_ = tree.return_code();// вот возможно фигня...
                std::cout << code_ << ' ';
                uint16_t bits_in_next_code = tree.bits_in_next_code();
                code_ <<= (16 - bits_in_next_code);
                while (bits_in_next_code) {
                    byte_to_write <<= 1;
                    byte_to_write |= ((code_ & 0b1000'0000'0000'0000) != 0);
                    // записать в младший бит байта на запись старший бит преобразованного кода

                    code_ <<= 1;

                    if (++cnt_ == 0) {
                        //std::cout << byte_to_write;
                        //std::cout.flush();
                        fout << byte_to_write;
                        byte_to_write = 0;
                    }
                    --bits_in_next_code;
                }
                /*
                for(uint16_t bits_remained = tree.bits_in_next_code(); bits_remained!= 0; --bits_remained){//пока в текущем коде остались биты для считывания
                    byte_to_write |=(code_ & 1);//запиши их в выходной байт
                    code_ >>= 1;//перейди в следующий бит
                    byte_to_write <<= 1;
                    if(++cnt_){
                        fout << byte_to_write;
                    }
                }
                 */
            }
        }
        //tree.insert(EOF);
        tree.flush();
        code_ = tree.return_code();
        std::cout << code_ << ' ';
        uint16_t bits_in_next_code = tree.bits_in_next_code();

        uint16_t bits_remained_in_general = bits_in_next_code + cnt_;
        uint16_t last_bit_will_remain = bits_remained_in_general % 8;

        code_ <<= (16 - bits_in_next_code);

        while (bits_remained_in_general) {
            byte_to_write <<= 1;
            byte_to_write |= ((code_ & 0b1000'0000'0000'0000) != 0);
            // записать в младший бит байта на запись старший бит преобразованного кода

            code_ <<= 1;

            if (++cnt_ == 0) {
//                std::cout << byte_to_write;
//                std::cout.flush();
                fout << byte_to_write;
                byte_to_write = 0;
            }
            --bits_remained_in_general;
        }

        byte_to_write <<= (8 - last_bit_will_remain);
        fout << byte_to_write;
    }
}


class DecoderLZW : protected CoderLZW {
private:

    Decoding_LZW_Tree tree;
    u_int16_t code_ = 0;

    struct buffer_of_32bit {// так как максимальный вес кода - 12 бит, переполнения не возникнет никогда
    private:
        counter cnt_ = counter(32);
        u_int32_t buff = 0;
    public:
        void push_back(u_char ch) {
            u_int32_t bit_mask = ch;
            bit_mask <<= (32 - (cnt_ + 8));
            buff |= bit_mask;
            cnt_ += 8;
        }

        uint16_t get_bits(u_char num) {
            uint32_t out = buff;
            /*uint32_t bit_mask = 4294967295; // = 2^32 - 1 = 1...(32times) in binary
            bit_mask <<= (32 - num);
            out &= bit_mask;
            */
            out &= (4294967295 << (32 - num));
            out >>= (32 - num);
            buff <<= num;
            cnt_ -= num;
            return out;
        }

        uint32_t size()
        const {
            return cnt_;
        }
    } buff;

public:
    DecoderLZW(const char *input_file, const char *output_file);
};

DecoderLZW::DecoderLZW(const char *input_file, const char *output_file) :
        CoderLZW(input_file, output_file) {
    if (fin.peek() != EOF) {
        for (char ch = 0; fin.get(ch);) {
            uint16_t bits_in_next_code = tree.request_bits();
            u_char current_byte = ch;
            buff.push_back(current_byte);
            if (buff.size() >= bits_in_next_code) {
                code_ = buff.get_bits(bits_in_next_code);// почему после выдачи 9 бит из 16 там осталось 8????
                tree.check_code(code_);//переставил эти две строки местами
                //bits_in_next_code = tree.request_bits();//переставил эти две строки местами

                std::string output = tree.string_matches_code();
                for (auto ch:output) {
                    //std::cout << ch; std::cout.flush();
                    fout << ch;
                }
            }
        }

    }
// считать 8 бит.
// Считать 9-й. Проверить, есть ли такой код в дереве.
// Если есть - проверить 10.
// если нет - вывести для 8
}

#endif //ARCHRONIS_LZWCODER_HPP
