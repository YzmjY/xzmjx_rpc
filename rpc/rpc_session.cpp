//
// Created by 20132 on 2022/11/30.
//

#include "rpc_session.h"

#include "utils/log.h"

namespace xzmjx {
namespace rpc {
std::shared_ptr<spdlog::logger> g_logger = log::GetDefault();
RpcSession::RpcSession(Socket::ptr socket, bool owner)
    : SocketStream(socket, owner) {}

Protocol::ptr RpcSession::recvProtocol() {
  Protocol::ptr msg = Protocol::Create();
  ByteArray::ptr bt = std::make_shared<ByteArray>();

  int n = readFixedSize(bt, Protocol::kHeaderLen);
  if (n == 0) {
    SPDLOG_LOGGER_INFO(g_logger, "socket closed,peer address = {}",
                       getPeerAddressString());
    return nullptr;
  }
  if (n < 0) {
    SPDLOG_LOGGER_ERROR(g_logger,
                        "cannot read protocol header, need least {} bytes",
                        Protocol::kHeaderLen);
    return nullptr;
  }

  bt->setPosition(0);
  msg->decodeHeader(bt);
  if (msg->getMagic() != Protocol::kMagic) {
    SPDLOG_LOGGER_WARN(g_logger, "protocol magic not match {}",
                       Protocol::kMagic);
    return nullptr;
  }
  if (msg->getVersion() != Protocol::kDefaultVersion) {
    SPDLOG_LOGGER_WARN(g_logger, "protocol version(version:{}) unsupported",
                       msg->getVersion());
    return nullptr;
  }

  uint64_t content_len = msg->getPayloadLength();
  if (content_len != 0) {
    bt->clear();
    if (readFixedSize(bt, msg->getPayloadLength()) <= 0) {
      SPDLOG_LOGGER_WARN(g_logger, "protocol payload receive failed");
      return nullptr;
    }
    bt->setPosition(0);
    if (msg->carryMsg()) {
      msg->setErrMsg(bt->readStringVint());
    }
    msg->setPayload(bt->toString());
  }
  return msg;
}

bool RpcSession::sendProtocol(Protocol::ptr msg) {
  SPDLOG_LOGGER_DEBUG(g_logger, "send protocol {}", msg->debugString());
  auto bt = msg->encode();
  std::unique_lock<co::co_mutex> lock(m_mutex);
  return writeFixedSize(bt, bt->getSize()) > 0;
}
}  // namespace rpc
}  // namespace xzmjx