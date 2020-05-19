#include <cstring>
#include "LZW.h"

int main(int argc, char **argv) {

    if (!strcmp(argv[1], "--encode")) {
        for (int i = 2; i <argc; ++i) {
            Encoder e(argv[i]);
        }

    } else if (!strcmp(argv[1], "--decode")) {
        for (int i = 2; i < argc; ++i) {
            Decoder d(argv[i]);
        }
    }

    return 0;
}
