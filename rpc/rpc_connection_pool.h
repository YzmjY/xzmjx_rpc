//
// Created by 20132 on 2022/12/7.
//

#ifndef XZMJX_RPC_CONNECTION_POOL_H
#define XZMJX_RPC_CONNECTION_POOL_H
#include <memory>
namespace xzmjx::rpc {
class RpcConnectionPool {
public:
  using ptr = std::shared_ptr<RpcConnectionPool>;

private:
};
} // namespace xzmjx::rpc

#endif // XZMJX_RPC_CONNECTION_POOL_H
