#include <cstring>
#include <iostream>
#include "Coder.h"
//#include "LZW.hpp"
#include "LZWcoder.hpp"
#include <vector>
//#include "LZW.h"
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

#include <utility>
#include <iostream>
#include <sstream>
#include "Socket.hpp"

#include "LZW_CS.hpp"

int main(int argc, char **argv) {

//argv[2]
    struct sockaddr_in addr;

    memset(&addr, 0x00, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3490);
    addr.sin_addr.s_addr = INADDR_ANY;


    try {
        ServerSocket server(&addr);

        server.bind();
        server.listen();

        std::cout << "Waiting for client" << std::endl;

        ClientSocket client = server.Accept();

        std::cout << "Client connected" << std::endl;

        client << "Hello, Client\n\r";
        Encoder(client, argv[2]);

        //Decoder(client, argv[2]);
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;



}
