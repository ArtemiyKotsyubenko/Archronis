#include <cstring>
#include "LZW.hpp"

int main(int argc, char **argv) {

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
}
