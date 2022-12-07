//
// Created by 20132 on 2022/12/2.
//
#include "net/socket.h"
#include "net/socketstream.h"
#include "stdio.h"
#include <iostream>

int main() {
    xzmjx::Address::ptr addr = xzmjx::IPv4Address::Create("127.0.0.1",8090);
    xzmjx::Socket::ptr socket = xzmjx::Socket::CreateTCP(addr);
    while(false == socket->connect(addr)){}
    int n = 1024;
    char* buffer = new char[n];
    FILE* f = stdin;
    while(true) {
        std::cin.getline(buffer,n);
        socket->send(buffer,1024);
        memset(buffer,0,1024);
        socket->recv(buffer,1024);
        std::cout<<buffer<<std::endl;
    }
}
