//
// Created by 20132 on 2022/11/30.
//

#ifndef XZMJX_PROTOCOL_H
#define XZMJX_PROTOCOL_H

#include <memory>
#include <string>

#include "spdlog/fmt/fmt.h"
#include "utils/byte_array.h"
namespace xzmjx::rpc {

/*
 * RPC传输报文协议封装
 */
class Context;
class Protocol {
 public:
  static constexpr uint8_t kDefaultVersion = 0x10;
  static constexpr uint8_t kMagic = 0xfc;
  static constexpr uint8_t kHeaderLen = 20;

  using ptr = std::shared_ptr<Protocol>;

  enum class ErrCode : uint8_t {
    kRpcDefault = 0,
    kRpcSuccess,
    kRpcNoMethod,
    kRpcParamNotMatch,
    kRpcMethodCallException,
    kRpcServerBusy,
    kRpcServerClosed,
    kRpcCallTimeout,
    kRpcRetTypeNotMatch,
    kRpcProtocolFormatError,
    kRpcUnknownError
  };

  enum class Type : uint8_t {
    kRpcMethodRequest = 0,
    kRpcMethodResponse,
    kRpcServiceDiscoverRequest,
    kRpcServiceDiscoverResponse,
    kRpcServiceProvider,
    kRpcServiceRegisterRequest,
    kRpcServiceRegisterResponse,
    kRpcHeartbeat,
  };

  static Protocol::ptr Create(Protocol::Type type, Context ctx,
                              const std::string& content, uint64_t id = 0);
  static Protocol::ptr Create(Protocol::Type type, const std::string& content,
                              uint64_t id = 0);
  static Protocol::ptr Create();

  static Protocol::ptr HeartbeatProtocol();

  void decodeHeader(ByteArray::ptr bt);
  void encodeHeader(ByteArray::ptr bt) const;

  ByteArray::ptr encode() const;

  std::string debugString() const {
    return fmt::format(
        "[magic = {}, m_version = {}, type = {},err_code = {}, m_seq = {}, "
        "length = {}, payload = {}]",
        m_magic, m_version, (uint8_t)m_type, (uint8_t)(m_err_code) >> 1, m_seq,
        m_payload_len, m_payload);
  }

 public:
  Protocol();
  void setMagic(uint8_t m) { m_magic = m; }
  void setVersion(uint8_t v) { m_version = v; }
  void setType(Protocol::Type t) { m_type = t; }
  void setErrCode(Protocol::ErrCode c) { m_err_code = c; }
  void setErrMsg(const std::string& msg) {
    if (msg.empty()) {
      return;
    }
    m_carry_msg = true;
    m_payload += msg.size();
    m_err_msg = msg;
  }
  void setSeqId(uint64_t id) { m_seq = id; }
  void setPayloadLength(uint64_t len) { m_payload_len += len; }
  void setPayload(const std::string& s) { m_payload = s; }

  uint8_t getMagic() const { return m_magic; }
  uint8_t getVersion() const { return m_version; }
  Protocol::Type getType() const { return m_type; }
  Protocol::ErrCode getErrCode() const { return m_err_code; }
  const std::string& getErrMsg() const { return m_err_msg; }
  uint64_t getSeqId() const { return m_seq; }
  uint64_t getPayloadLength() const { return m_payload_len - m_err_msg.size(); }
  const std::string& getPayload() const { return m_payload; }

  bool carryMsg() const { return m_carry_msg; }

 private:
  bool m_carry_msg;
  uint8_t m_magic;
  uint8_t m_version;
  Type m_type;
  ErrCode m_err_code;
  uint64_t m_seq;
  uint64_t m_payload_len;
  std::string m_err_msg;
  std::string m_payload;
};
using RPCErrorCode = xzmjx::rpc::Protocol::ErrCode;
}  // namespace xzmjx::rpc

#endif  // XZMJX_PROTOCOL_H
