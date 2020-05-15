#include <cstring>
#include <iostream>
#include "Coder.h"
#include "LZW.hpp"
#include "LZWcoder.hpp"
#include <vector>

int main(int argc, char **argv) {
//int main() {

/*
//    std::ios_base::binary;
//    if (argc != 4) {
//        std::cerr << "Wrong format" << std::endl;
//        exit(1);
//    } else if (!strcmp(argv[3], "--encode")) {
//        HuffmanEncoder(argv[1], argv[2]);
//
//    } else if (!strcmp(argv[3], "--decode")) {
//        HuffmanDecoder(argv[1], argv[2]);
//    } else {
//        std::cerr << "wrong format" << std::endl;
//        exit(1);
//    }
 */

    if (argc != 4) {
        std::cerr << "Wrong format" << std::endl;
        exit(1);
    } else if (!strcmp(argv[3], "--encode")) {
        EncoderLZW(argv[1], argv[2]);

    } else if (!strcmp(argv[3], "--decode")) {
        DecoderLZW(argv[1], argv[2]);
    } else {
        std::cerr << "wrong format" << std::endl;
        exit(1);
    }

    return 0;

/*
Encoding_LZW_Tree tree;
std::string str = "abacabadabacabae";
std::string output;
for(auto s : str){

    if(s == 'a'){
        s = 0;
    }else if(s== 'b'){
        s = 1;
    }else if(s== 'c'){
        s = 2;
    }else if(s== 'd'){
        s = 3;
    }else if(s== 'e'){
        s = 4;
    }

    if (tree.insert(s)){
        //output+= tree.write();
        std::cout << tree.bits_in_next_code();
    }

}
    if (tree.insert(EOF)) {
        //output += tree.write();
        std::cout << tree.bits_in_next_code();
    }

for(auto  it: output){
    std::cout << static_cast<int>(it);
}
//std::cout << output;

//    Decoding_LZW_Tree tree;
//    std::vector<u_int16_t> vec = {0, 1, 0, 2, 5, 0, 3, 9, 8, 6, 4};
//
//    for(auto  num: vec){
//        tree.check_code(num);
//        auto  str = tree.string_matches_code();
//        for(auto s: str){
//            if(s ==0){
//                std::cout << 'a';
//            } else if(s ==1){
//                std::cout << 'b';
//            } else if(s ==2){
//                std::cout << 'c';
//            }else if(s ==3){
//                std::cout << 'd';
//            }else if(s ==4){
//                std::cout << 'e';
//            }
//        }
//    }
*/


}
//source.txt arch.txt --encode
//arch.txt decoded.txt --decode




/*
 * Идея - хранить в дереве не более N элементов.
 * Как только вставляется N+1, самый тяжелый элемент(он брат разделителя удалятся из дерева и из листа)
 * Доступ для удаления из листа - по итератору, который хранится в ячейке вектора с индексом с номером [symbol]
 * */