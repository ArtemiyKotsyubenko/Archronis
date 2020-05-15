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
            return (i += 8) %= mod;
        }

        const int operator-=(const u_char num) {
            return (i -= 8) %= mod;
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
            if (tree.insert(ch)) {// возможно ниже неправильная реализация
                code_ = tree.return_code();// вот возможно фигня...
                code_ << (16 - tree.bits_in_next_code());
                /*//записать в bit_to write
                 //когда он переполнится - записать его в файл
                uint16_t bits_remained = tree.bits_in_next_code();
                while (bits_remained){
                    byte_to_write |=(code_ & 1);
                    code_ <<= 1;
                    byte_to_write <<= 1;
                    if(++cnt_){
                        fout << byte_to_write;
                    }
                }*/
                while (code_) {
                    byte_to_write |= ((code_ & 0b1000'0000'0000'0000) != 0);
                    // записать в младший бит байта на запись старший бит преобразованного кода

                    code_ <<= 1;
                    byte_to_write <<= 1;
                    if (++cnt_) {
                        fout << byte_to_write;
                    }
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
        tree.insert(EOF);
        code_ = tree.return_code();// вот возможно фигня...
        code_ << (16 - tree.bits_in_next_code());
        while (code_) {
            byte_to_write |= ((code_ & 0b1000'0000'0000'0000) != 0);
            // записать в младший бит байта на запись старший бит преобразованного кода

            code_ <<= 1;
            byte_to_write <<= 1;
            if (++cnt_) {
                fout << byte_to_write;
            }
        }
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

        uint32_t size() const {
            return cnt_;
        }
    } buff;

public:
    DecoderLZW(const char *input_file, const char *output_file);
};

DecoderLZW::DecoderLZW(const char *input_file, const char *output_file) :
        CoderLZW(input_file, output_file) {
    if (fin.peek() != EOF) {
        u_int16_t bits_in_next_code = tree.request_bits();

        for (char ch = 0; fin.get(ch);) {
            u_char current_byte = ch;
            buff.push_back(current_byte);
            if (buff.size() >= bits_in_next_code) {
                code_ = buff.get_bits(bits_in_next_code);
                bits_in_next_code = tree.request_bits();
                tree.check_code(code_);
                std::string output = tree.string_matches_code();
                for (auto ch:output) {
                    fout << output;
                }
            }
        }

//        tree.check_code(EOF);
//        std::string output = tree.string_matches_code();
//        for (auto ch:output) {
//            fout << output;
//        }
    }

}

#endif //ARCHRONIS_LZWCODER_HPP
