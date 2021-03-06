//
// Created by artemiy on 17.05.2020.
//

#ifndef ARCHRONIS_LZW_H
#define ARCHRONIS_LZW_H

#include <unordered_map>
#include "Streams.h"
#include <vector>
#include <memory>
#include "Adapter.h"

class Coder {
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

    std::ifstream &fin;
    std::ofstream &fout;

    uint16_t code_ = 0;
    u_char byte_to_write = 0;

    Coder(const char *input_file, const char *output_file) :
            Fin(input_file),
            Fout(output_file),
            fin(Fin.file),
            fout(Fout.file) {}

    ~Coder() = default;
};

class Encoder : protected Coder {
private:
    std::unordered_map<std::string, uint16_t> dict;
    std::string previous;
    //std::string current;
    u_char current;
    u_char previous_byte;
    counter cnt_ = counter(8);

    u_int16_t bits_in_next() {
        unsigned cnt = 0;
        for (unsigned copy = dict.size() - 1; copy != 0; copy >>= 1, ++cnt);
        return cnt < 8 ? 8 : cnt;
    }

public:
    Encoder(const char *input_file, const char *output_file);


};

Encoder::Encoder(const char *input_file, const char *output_file) : Coder(input_file, output_file) {

    for (int i = 0; i < 256; ++i) {
        dict.emplace(std::string(1, i), i);
    }


    if (fin.peek() != EOF) {// коды верны. Верна ли битовая запись?
        previous.push_back(fin.get());

        for (char ch; fin.get(ch);) {
            current = ch;
            std::string concat(previous);
            concat.push_back(current);

            auto it = dict.find(concat);
            if (it != dict.end()) {
                previous = concat;
                continue;
            }
            code_ = dict[previous];
            uint16_t bits_in_next_code = bits_in_next();
            code_ <<= (16 - bits_in_next_code);

            while (bits_in_next_code) {
                byte_to_write <<= 1;
                byte_to_write |= ((code_ & 0b1000'0000'0000'0000) != 0);
                code_ <<= 1;

                if (++cnt_ == 0) {
                    fout << byte_to_write;
                    byte_to_write = 0;
                }
                --bits_in_next_code;
            }

            dict.insert({concat, dict.size() + 1});

            previous.clear();
            previous.push_back(current);
        }

        code_ = dict[previous];

        uint16_t bits_in_next_code = bits_in_next();
        uint16_t bits_remained_in_general = bits_in_next_code + cnt_;
        uint16_t last_bit_will_remain = bits_remained_in_general % 8;

        code_ <<= (16 - bits_in_next_code);

        while (bits_remained_in_general) {
            byte_to_write <<= 1;
            byte_to_write |= ((code_ & 0b1000'0000'0000'0000) != 0);
            code_ <<= 1;
            if (++cnt_ == 0) {
                fout << byte_to_write;
                byte_to_write = 0;
            }
            --bits_remained_in_general;
        }
        byte_to_write <<= (8 - last_bit_will_remain);
        fout << byte_to_write;
    }
}


class Decoder : protected Coder {
private:
    std::unordered_map<uint16_t, std::string> dict;
    std::unordered_map<std::string, uint16_t> dict_;
    uint16_t current_code;
    uint16_t previous_code;
    std::string current_string;
    std::string previous_string;

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
            out &= (UINT16_MAX << (32 - num));
            out >>= (32 - num);
            buff <<= num;
            cnt_ -= num;
            return out;
        }

        uint32_t size() const {
            return cnt_;
        }

    } buff;

    u_int16_t bits_in_next() {
        unsigned cnt = 0;
        for (unsigned copy = dict.size(); copy != 0; copy >>= 1, ++cnt);
        return cnt < 8 ? 8 : cnt;
    }

public:
    Decoder(const char *input_file, const char *output_file);
};

Decoder::Decoder(const char *input_file, const char *output_file) : Coder(input_file, output_file) {


    for (int i = 0; i < 256; ++i) {
        dict.emplace(i, std::string(1, i));
    }


    if (fin.peek() != EOF) {
        previous_code = fin.get();
        previous_string = dict[previous_code];
        fout << previous_string;

        for (char ch = 0; fin.get(ch);) {

            u_char u_ch = ch;

            uint16_t bits_in_next_code = bits_in_next();
            buff.push_back(u_ch);
            if(buff.size() >= bits_in_next_code) {
                current_code = buff.get_bits(bits_in_next_code);
                if (dict.find(current_code) != dict.end()) {
                    std::string output = dict[current_code];
                    for (auto ch: output) {
                        fout << ch;
                    }

                    previous_string = dict[previous_code];

                    current_string = dict[current_code][0];

                    dict.emplace(dict.size() + 1, previous_string + current_string);

                } else {
                    previous_string = dict[previous_code];
                    current_string.clear();
                    current_string.push_back(dict[previous_code][0]);

                    std::string output = previous_string + current_string;
                    for (auto ch: output) {
                        fout << ch;
                    }
                    dict.emplace(current_code, output);
                }
                previous_code = current_code;
            }
        }
    }
}

#endif //ARCHRONIS_LZW_H
