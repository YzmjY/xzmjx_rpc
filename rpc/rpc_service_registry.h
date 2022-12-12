//
// Created by 20132 on 2022/12/3.
//

#ifndef XZMJX_RPC_REGISTRY_H
#define XZMJX_RPC_REGISTRY_H
#include <map>
#include <memory>
#include <string>

#include "net/address.h"
#include "net/tcp_server.h"
#include "protocol.h"
#include "rpc_session.h"
namespace xzmjx::rpc {
class RpcServiceRegistry : public TcpServer {
 public:
  using ptr = std::shared_ptr<RpcServiceRegistry>;
  using RWMutexType = co_rwmutex;

  void handleClient(Socket::ptr client) final;

 private:
  void unregisterService(const std::string& server_addr);
  Protocol::ptr handleRegisterService(const std::string& addr,
                                      Protocol::ptr req);
  std::string handleProvider(RpcSession::ptr session, Protocol::ptr req);
  Protocol::ptr handleHeartbeat();
  Protocol::ptr handleServiceDiscover(Protocol::ptr req);

 private:
  std::multimap<std::string, std::string> m_services_provider;
  std::multimap<std::string, std::string> m_address_to_service;
  RWMutexType m_rwmutex;
};
}  // namespace xzmjx::rpc

#endif  // XZMJX_RPC_REGISTRY_H
