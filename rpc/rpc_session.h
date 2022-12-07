//
// Created by 20132 on 2022/11/30.
//

#ifndef XZMJX_RPCSESSION_H
#define XZMJX_RPCSESSION_H
#include "net/socketstream.h"
#include "protocol.h"
#include "libgo.h"

namespace xzmjx {
namespace rpc {
class RpcSession : public SocketStream{
public:
    using ptr = std::shared_ptr<RpcSession>;
    using MutexType = co::co_mutex;

    RpcSession(Socket::ptr socket,bool owner = true);

    Protocol::ptr recvProtocol();
    bool sendProtocol(Protocol::ptr msg);
private:
    MutexType m_mutex; // 保护复用连接
};

}
}


#endif //XZMJX_RPCSESSION_H
