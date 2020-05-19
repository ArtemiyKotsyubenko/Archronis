#include <cstring>
#include "HuffmanCoder.hpp"


int main(int argc, char **argv) {

    if (argc != 4) {
        std::cerr << "Wrong format" << std::endl;
        exit(1);
    } else if (!strcmp(argv[3], "--encode")) {
        HuffmanEncoder(argv[1], argv[2]);

    } else if (!strcmp(argv[3], "--decode")) {
        HuffmanDecoder(argv[1], argv[2]);
    } else {
        std::cerr << "wrong format" << std::endl;
        exit(1);
    }

    return 0;
}
