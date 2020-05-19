//
// Created by artemiy on 19.05.2020.
//
#include "Socket.hpp"
#include "Streams.h"
#include <iostream>
#include <string>
#include <cstring>
#include <exception>
#include <ios>
#include <netinet/in.h>
#include <iostream>
#include <utility>

#include <sys/types.h>
#include <sys/socket.h>

#include <cerrno>
#include <cstring>

#include <stdexcept>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <utility>
#include <iostream>
#include <sstream>

int main(int argc, char **argv) {


    struct sockaddr_in addr;

    memset(&addr, 0x00, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3490);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    ClientSocket client(&addr);
    client.connect();

    client << std::string(argv[2]);// sending option
    client << std::string (argv[1]);// sending filename

    try {
        Ifstream_wrap fin(argv[1]);
        if (fin.file.peek() != EOF) {
            for (char ch; fin.file.get(ch);) {
                client << ch;
            }
        }
        client.shutdown();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
/*

    if (argc != 4) {
        std::cerr << "Wrong format" << std::endl;
        exit(1);
    } else if (!strcmp(argv[3], "--encode")) {

    } else if (!strcmp(argv[3], "--decode")) {
        try {
            Ifstream_wrap fin(argv[1]);
            if (fin.file.peek() != EOF) {
                for (char ch; fin.file.get(ch);) {
                    client << ch;
                }
            }
            client.shutdown();
        }
        catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    } else {
        std::cerr << "wrong format" << std::endl;
        exit(1);
    }
*/


}
// f i l e . e x e '\0'
// 0 1 2 3 4 5 6 7  8