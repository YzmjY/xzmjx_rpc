//
// Created by 20132 on 2022/11/30.
//

#include "protocol.h"

#include "context.h"

namespace xzmjx::rpc {
Protocol::Protocol()
    : m_carry_msg(false),
      m_magic(kMagic),
      m_version(kDefaultVersion),
      m_type{},
      m_err_code{ErrCode::kRpcSuccess},
      m_seq(0),
      m_payload_len(0) {}

Protocol::ptr Protocol::Create(Protocol::Type type, const std::string &content,
                               uint64_t id) {
  Context ctx;
  ctx.setCode(RPCErrorCode::kRpcSuccess);
  return Create(type, std::move(ctx), content, id);
}

Protocol::ptr Protocol::Create(Protocol::Type type, Context ctx,
                               const std::string &content, uint64_t id) {
  Protocol::ptr ptr = std::make_shared<Protocol>();
  ptr->setType(type);
  ptr->setErrCode(ctx.getCode());
  ptr->setErrMsg(ctx.getMsg());
  ptr->setPayloadLength(content.size());
  ptr->setPayload(content);
  ptr->setSeqId(id);
  return ptr;
}
Protocol::ptr Protocol::Create() {
  Protocol::ptr ptr = std::make_shared<Protocol>();
  return ptr;
}

Protocol::ptr Protocol::HeartbeatProtocol() {
  Protocol::ptr ptr = std::make_shared<Protocol>();
  ptr->setType(Type::kRpcHeartbeat);
  return ptr;
}

void Protocol::decodeHeader(ByteArray::ptr bt) {
  m_magic = bt->readFint8();
  m_version = bt->readFint8();
  m_type = (Type)(bt->readFint8());
  uint8_t temp = bt->readFint8();
  m_err_code = (ErrCode)(temp >> 1);
  m_carry_msg = (bool)(temp & 1);
  m_seq = bt->readFint64();
  m_payload_len = bt->readFint64();
}

void Protocol::encodeHeader(ByteArray::ptr bt) const {
  uint8_t has_err_msg = m_carry_msg ? 1 : 0;
  bt->writeFuint8(m_magic);
  bt->writeFuint8(m_version);
  bt->writeFuint8((uint8_t)m_type);
  bt->writeFuint8(((uint8_t)m_err_code << 1) | has_err_msg);
  bt->writeFuint64(m_seq);
  bt->writeFuint64(m_payload_len);
}

ByteArray::ptr Protocol::encode() const {
  ByteArray::ptr bt = std::make_shared<ByteArray>();
  encodeHeader(bt);
  if (m_carry_msg) {
    bt->writeStringVint(m_err_msg);
  }

  std::size_t n = m_payload_len - m_err_msg.size();
  if (n != 0) {
    bt->write(m_payload.c_str(), n);
  }
  bt->setPosition(0);
  return bt;
}
} // namespace xzmjx::rpc
