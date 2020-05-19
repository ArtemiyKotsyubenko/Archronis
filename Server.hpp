//
// Created by artemiy on 19.05.2020.
//

#ifndef ARCHRONIS_SERVER_HPP
#define ARCHRONIS_SERVER_HPP

#include "Server.hpp"
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
#include <queue>
#include "LZW_CS.hpp"

class Server {
private:
    const unsigned max_queue_size = 10;
    bool is_running = false;
    ServerSocket sock;
    std::queue<ClientSocket> waiters;

    std::string split_file_ext(std::string &filename) {
        int i = filename.length();
        for (; filename[i - 1] != '.'; --i);
        std::string ext;
        for (int j = i; j < filename.length(); ++j) {
            ext.push_back(filename[j]);
        }
        for (i = filename.length() - i; i > -1; --i) {
            filename.pop_back();
        }
        return ext;
    }

public:
    void start();

    void run();

    void stop();
};

void Server::start() {
    struct sockaddr_in addr;
    memset(&addr, 0x00, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3490);
    addr.sin_addr.s_addr = INADDR_ANY;

    sock.bind();
    sock.listen();
}

void Server::run() {
    is_running = true;
    std::cout << "running";
    std::cout.flush();

    unsigned clients_processed = 0;

    auto handler_func = [&](ClientSocket &current_client) {
        //ClientSocket current_client = std::move(new_client);
        std::string option;
        current_client >> option;

        std::string file_name;
        current_client >> file_name;

        if (option == "--decode") {
            Decoder(current_client, file_name.c_str());
        } else if (option == "--encode") {
            Encoder(current_client, file_name.c_str());
        }

        std::cout << "complete";
        std::cout.flush();


    };

    while (is_running) {
        ClientSocket client = sock.Accept();
        //handler_func(std::move(client));
        handler_func(client);
        std::cout <<"\ncompleted";
        std::cout.flush();
    }


}

void Server::stop() {
    is_running = false;
    sock.shutdown();
}


#endif //ARCHRONIS_SERVER_HPP
