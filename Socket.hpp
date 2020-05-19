//
// Created by artemiy on 18.05.2020.
//

#ifndef ARCHRONIS_SOCKET_HPP
#define ARCHRONIS_SOCKET_HPP

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

class Socket {
private:
    std::stringstream in_buf;
    std::stringstream out_buf;
    int sock_fd;
protected:
    bool is_available = false;
public:
    Socket();

    Socket(int sock);

    virtual ~Socket() = 0;

    Socket(const Socket &) = delete;

    Socket(Socket &&other) noexcept;

    void receive(char *data, std::streamsize size);

    void send(const char *data, std::streamsize size);

    Socket &operator>>(std::string &str);

    Socket &operator<<(const std::string &str);

    Socket& operator>>(char& ch);

    Socket& operator<<(char& ch);

    void shutdown();

    int get_sock_fd() const;
};


int Socket::get_sock_fd() const {
    return sock_fd;
}

Socket::Socket() {
    is_available = false;
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        throw std::runtime_error("Socket::Socket(): socket() - error");
    }
}

Socket::Socket(int sock)
        : sock_fd(sock) {}

Socket::~Socket() {
    shutdown();
}

//Socket::Socket(Socket &&other) noexcept:
//        sock_fd(std::exchange(other.sock_fd, -1)),
//        is_available(std::exchange(other.is_available, false)) {}

Socket::Socket(Socket &&other) noexcept :
        sock_fd(other.sock_fd), is_available(other.is_available){}

void Socket::receive(char *data, std::streamsize size) {
    if (is_available) {
        if ((recv(sock_fd, data, size, 0)) == -1) {
            throw std::runtime_error("Socket::receive(): recv() - error");
        }

    } else {
        throw std::invalid_argument("Socket::receive(): is_available = false");
    }

}

void Socket::send(const char *data, std::streamsize size) {
    if (is_available) {
        if (::send(sock_fd, data, size, 0) == -1)
            throw std::runtime_error("Socket::send(): send() - error");
    } else
        throw std::invalid_argument("Socket::send(): is_available = false");

}

Socket &Socket::operator>>(std::string &str) {
    if (is_available) {
        bool CR = false; // Для '\r' в обработке HTTP запроса
        bool LF = false; // Для '\n' в обработке HTTP запроса

        char byte = 0;

        str.clear();

        while (true) {
            if ((recv(sock_fd, &byte, 1, 0)) == -1) {
                throw std::runtime_error("Socket::operator>>(std::string&): recv() - error");
            }

            if (byte == '\r') {
                CR = true;
            } // Для обработки HTTP запроса добавляем '\r' к строке
            if (byte == '\n') {
                LF = true;
            } // Для обработки HTTP запроса добавляем '\n' к строке

            str.push_back(byte);

            if (CR & LF) {
                break;
            } // Строка завершающаяся символами '\r' и '\n' считана из сокета
        }
    } else
        throw std::invalid_argument("Socket::receive(): is_available = false");

    return *this;
}

Socket &Socket::operator<<(const std::string &str) {
    send(str.c_str(), str.length());
    return *this;
}

void Socket::shutdown() {
    if (sock_fd != -1) {
        if (::shutdown(sock_fd, 2) == -1) {
            if (errno != ENOTCONN) {
                std::string txt("Socket::shutdown(): shutdown() - ");
                std::string err(strerror(errno));
                throw std::runtime_error(txt + err);
            }
        }
    }
    is_available = false;
}

Socket &Socket::operator>>(char& ch) {
    if ((recv(sock_fd, &ch, 1, 0)) == 0) {
        throw std::runtime_error("Socket::operator>>(std::string&): recv() - error");
    }
    return *this;
}

Socket &Socket::operator<<(char &ch) {
    send(&ch, 1);
    return *this;
}





//******************************************************/




class ClientSocket : public Socket {
private:
    struct sockaddr_in peer;
public:
    ClientSocket();

    ClientSocket(struct sockaddr_in *addr);

    ClientSocket(int sock, struct sockaddr_in *addr);

    ClientSocket(const ClientSocket &) = delete;

    ClientSocket(ClientSocket &&other) noexcept;

    void connect();
};

ClientSocket::ClientSocket() {
    memset(&peer, 0x00, sizeof(struct sockaddr_in));
}

ClientSocket::ClientSocket(struct sockaddr_in *addr) {
    memcpy(&peer, addr, sizeof(struct sockaddr_in));
}

ClientSocket::ClientSocket(int sock, struct sockaddr_in *addr)
        : Socket(sock) {
    memcpy(&peer, addr, sizeof(struct sockaddr_in));

    is_available = true;
}

ClientSocket::ClientSocket(ClientSocket &&other) noexcept
        : Socket(std::move(other)) {
    //peer = other.peer;
    memcpy(&peer, &(other.peer), sizeof(struct sockaddr_in));
    memset(&(other.peer), 0x00, sizeof(struct sockaddr_in));
}

void ClientSocket::connect() {
    if (::connect(get_sock_fd(), (struct sockaddr *) &peer, sizeof(struct sockaddr_in)) == -1) {
        throw std::runtime_error("ClientSocket::connect(): connect() - error");
    }

    is_available = true;
}




//******************************************************/

class ServerSocket : public Socket {
private:
    struct sockaddr_in self;
public:
    ServerSocket();

    ServerSocket(struct sockaddr_in *addr);

    //void set_addr(struct sockaddr_in *addr); // Для HTTPServer

    void bind();

    void listen();

    ClientSocket Accept();

    void set_addr(struct sockaddr_in *addr); // Для HTTPServer
};

ServerSocket::ServerSocket() {
    memset(&self, 0x00, sizeof(struct sockaddr_in));
}

ServerSocket::ServerSocket(struct sockaddr_in *addr) {
    memcpy(&self, addr, sizeof(struct sockaddr_in));
}

void ServerSocket::set_addr(struct sockaddr_in *addr) {
    memcpy(&self, addr, sizeof(struct sockaddr_in));
}

void ServerSocket::bind() {
    if (::bind(get_sock_fd(), (struct sockaddr *) &self, sizeof(struct sockaddr_in)) == -1)
        throw std::runtime_error("ServerSocket::bind(): bind() - error");
}

void ServerSocket::listen() {
    if (::listen(get_sock_fd(), 0) == -1)
        throw std::runtime_error("ServerSocket::listen(): listen() - error");

    is_available = true;
}

ClientSocket ServerSocket::Accept() {
    if (is_available) {
        int cli_sock;
        struct sockaddr_in cli_addr;
        socklen_t addrlen = sizeof(struct sockaddr_in);

        if ((cli_sock = accept(get_sock_fd(), (struct sockaddr *) &cli_addr, &addrlen)) == -1)
            throw std::runtime_error("ServerSocket::Accept(): accept() - error");

        ClientSocket client(cli_sock, &cli_addr);

        return std::move(client);
    } else
        throw std::invalid_argument("ServerSocket::Accept(): good = false");
}


#endif //ARCHRONIS_SOCKET_HPP
