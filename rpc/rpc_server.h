//
// Created by 20132 on 2022/11/30.
//

#ifndef XZMJX_RPC_SERVER_H
#define XZMJX_RPC_SERVER_H
#include <map>
#include "context.h"
#include "net/tcp_server.h"
#include "rpc_session.h"
#include "serializer.h"
#include "spdlog/fmt/fmt.h"
#include "utils/function_traits.h"
namespace xzmjx::rpc {
class RpcServer : public TcpServer {
 public:
  using ptr = std::shared_ptr<RpcServer>;
  using FuncType = std::function<std::pair<Serializer, Context>(const std::string& name,
                                                                const std::string& args)>;

  explicit RpcServer(co::Scheduler* worker = &co::Scheduler::getInstance(),
                     co::Scheduler* accept_worker = &co::Scheduler::getInstance());

  bool bind(Address::ptr addr) override;

  bool bindRegistry(Address::ptr addr);

  bool start() override;

  void handleClient(Socket::ptr client) final;

  template <class Func>
  void registerMethod(const std::string& name, Func f) {
    FuncType real_call = [this, f](const std::string& name,
                                   const std::string& args) -> std::pair<Serializer, Context> {
      return internal_method_impl<Func>(f, name, args);
    };
    m_services[name] = real_call;
  }

 private:
  template <class Func>
  std::pair<Serializer, Context> internal_method_impl(Func f, const std::string& name,
                                                      const std::string& args) {
    using arg_tuple = typename function_traits<Func>::arg_tuple_type;
    using ret_type = typename function_traits<Func>::ret_type;

    Serializer s(args);
    Serializer res;
    Context ctx;

    do {
      arg_tuple arg_tp;
      try {
        s >> arg_tp;
      } catch (...) {
        ctx.setCode(RPCErrorCode::kRpcParamNotMatch);
        ctx.setMsg("args not match");
        break;
      }

      constexpr std::size_t arg_cnt = std::tuple_size_v<arg_tuple>;
      auto invoke = [&f, &arg_tp ]<std::size_t... Index>(std::index_sequence<Index...>) {
        return f(std::get<Index>(arg_tp)...);
      };
      if constexpr (std::is_same_v<void, ret_type>) {
        try {
          invoke(std::make_index_sequence<arg_cnt>{});
        } catch (std::exception& ex) {
          ctx.setCode(RPCErrorCode::kRpcMethodCallException);
          ctx.setMsg(ex.what());
          break;
        }

      } else {
        try {
          ret_type rt;
          rt = invoke(std::make_index_sequence<arg_cnt>{});
          res << rt;
        } catch (std::exception& ex) {
          res.clear();
          ctx.setCode(RPCErrorCode::kRpcMethodCallException);
          ctx.setMsg(ex.what());
          break;
        }
      }
      ctx.setCode(RPCErrorCode::kRpcSuccess);
    } while (0);

    return {res, ctx};
  }

 private:
  Protocol::ptr handleMethodCall(Protocol::ptr req);
  Protocol::ptr handleHeartbeat();
  void registerServiceToRegistry(const std::string& service);

  void handleRegistrySessionRecv();
  void handleRegistrySessionSend();

 private:
  /// 服务列表
  std::map<std::string, FuncType> m_services;

  /// 长连接，超时定时器
  co_timer m_client_timer;

  /// 注册中心定时器
  co_timer_id m_register_timer_id{};
  co_timer m_register_timer;
  bool m_registry_heartbeat_rsp;
  /// 客户端超时时间
  uint64_t m_client_alive_time;

  /// 服务端口
  uint32_t m_port{};

  /// 注册中心
  RpcSession::ptr m_registry_session;
  co_chan<Protocol::ptr> m_registry_send_chan;
};
}
#endif  // XZMJX_RPC_SERVER_H
