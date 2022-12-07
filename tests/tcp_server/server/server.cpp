//
// Created by 20132 on 2022/12/2.
//
#include "net/tcp_server.h"
#include "net/socketstream.h"
class EchoServer : public xzmjx::TcpServer {
public:
    using ptr = std::shared_ptr<EchoServer>;
    void handleClient(xzmjx::Socket::ptr socket) final {
        xzmjx::SocketStream::ptr stream = std::make_shared<xzmjx::SocketStream>(socket, true);
        char buffer[1024];
        while(true) {
            if(stream->read(buffer,1024)<0) {
                break;
            }
            stream->write(buffer,1024);
        }
    }
};

int main() {
    EchoServer::ptr server = std::make_shared<EchoServer>();
    xzmjx::Address::ptr addr = xzmjx::IPv4Address::Create("127.0.0.1",8090);
    server->bind(addr);
    server->start();
}
