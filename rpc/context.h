//
// Created by 20132 on 2022/12/5.
//

#ifndef XZMJX_CONTEXT_H
#define XZMJX_CONTEXT_H
#include "protocol.h"

namespace xzmjx::rpc {
class Context {
public:
  using ptr = std::shared_ptr<Context>;
  Context() : m_code(RPCErrorCode::kRpcDefault) {}
  Context& operator=(const Context&) = default;
  Context(const Context&) = default;
  Context(Context&& ctx) : m_code(ctx.m_code), m_msg(std::move(ctx.m_msg)) {}

  void setCode(RPCErrorCode code) { m_code = code; }
  void setMsg(const std::string& msg) { m_msg = msg; }

  RPCErrorCode getCode() const { return m_code; }
  const std::string& getMsg() const { return m_msg; }

private:
  RPCErrorCode m_code;
  std::string m_msg;
};
} // namespace xzmjx::rpc

#endif // XZMJX_CONTEXT_H
