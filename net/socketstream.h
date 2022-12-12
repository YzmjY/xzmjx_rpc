//
// Created by 20132 on 2022/11/29.
//

#ifndef XZMJX_SOCKETSTREAM_H
#define XZMJX_SOCKETSTREAM_H
#include <memory>

#include "socket.h"
#include "stream.h"
namespace xzmjx {
class SocketStream : public Stream {
 public:
  typedef std::shared_ptr<SocketStream> ptr;
  SocketStream(Socket::ptr sock, bool owner);
  ~SocketStream();

  int read(void* buf, size_t length) override;
  int read(ByteArray::ptr ba, size_t length) override;
  int write(const void* buf, size_t length) override;
  int write(ByteArray::ptr ba, size_t length) override;
  void close() override;
  Socket::ptr getSocket() const { return m_socket; }
  bool isConnected() const;
  Address::ptr getLocalAddress();
  Address::ptr getPeerAddress();
  std::string getLocalAddresString();
  std::string getPeerAddressString();

 private:
  Socket::ptr m_socket;
  bool m_owner;
};
}  // namespace xzmjx

#endif  // XZMJX_SOCKETSTREAM_H
