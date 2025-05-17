#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <iostream>

#define PORT 8888

#define err(errMsg) do { \
    std::cerr << "[line: " << __LINE__ << "] " << errMsg << " failed: " << errno << " (" << strerror(errno) << ")" << std::endl; \
} while(0)

bool InitSocket();

bool CloseSocket(int socket_fd);

int CreatServerSocket();

int CreatClientSocket(const char* IP);

#endif