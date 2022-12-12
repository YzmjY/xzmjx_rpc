//
// Created by 20132 on 2022/12/1.
//

#include "rpc_client.h"

#include "net/socket.h"
#include "utils/config.h"
#include "utils/log.h"
namespace xzmjx::rpc {
xzmjx::ConfigVar<uint64_t>::ptr g_channel_size = Config::Lookup(
    "rpc_client.channel_size", (uint64_t)(200), "rpc client channel size");
xzmjx::ConfigVar<uint64_t>::ptr g_connect_timeout = Config::Lookup(
    "rpc_client.connect_timeout", (uint64_t)(10), "rpc client connect timeout");
xzmjx::ConfigVar<uint64_t>::ptr g_heartbeat_timeout =
    Config::Lookup("rpc_client.heartbeat_timeout", (uint64_t)(30),
                   "rpc client heartbeat timeout");
xzmjx::ConfigVar<uint64_t>::ptr g_method_call_timeout =
    Config::Lookup("rpc_client.method_call_timeout", (uint64_t)(5),
                   "rpc client method call timeout");

static auto g_logger = log::GetDefault();
RpcClient::RpcClient(co::Scheduler *worker)
    : m_chan(g_channel_size->getValue()),
      m_heartbeat_timer(std::chrono::milliseconds(1), worker),
      m_conn_timeout(g_connect_timeout->getValue()),
      m_heartbeat_timeout(g_heartbeat_timeout->getValue()),
      m_call_timeout(g_method_call_timeout->getValue()) {}

RpcClient::~RpcClient() { close(); }

bool RpcClient::connect(Address::ptr address) {
  ///连接服务器
  Socket::ptr socket = Socket::CreateTCP(address);
  if (!socket->connect(address, m_conn_timeout)) {
    SPDLOG_LOGGER_ERROR(g_logger, "connect to {} timeout", address->toString());
    return false;
  }

  SPDLOG_LOGGER_INFO(g_logger, "connected to {}", address->toString());
  m_is_closed = false;
  m_session = std::make_shared<RpcSession>(socket);
  go co_scheduler(m_worker)[this]() { handleSend(); };
  go co_scheduler(m_worker)[this]() { handleRecv(); };

  m_heartbeat_cb = [this]() {
    if (!m_is_heartbeat_response) {
      SPDLOG_LOGGER_WARN(g_logger, "server closed");
      close();
      return;
    }
    auto heartbeat = Protocol::HeartbeatProtocol();
    SPDLOG_LOGGER_INFO(g_logger, "heartbeat");
    m_chan << heartbeat;
    m_is_heartbeat_response = false;
    m_heartbeat_timer_id = m_heartbeat_timer.ExpireAt(
        std::chrono::seconds(m_heartbeat_timeout), m_heartbeat_cb);
  };
  m_is_heartbeat_response = true;
  m_heartbeat_timer_id = m_heartbeat_timer.ExpireAt(
      std::chrono::seconds(m_heartbeat_timeout), m_heartbeat_cb);
  return true;
}

void RpcClient::handleSend() {
  Protocol::ptr msg;
  while (m_chan.pop(msg)) {
    if (false == m_session->sendProtocol(msg)) {
      SPDLOG_LOGGER_ERROR(g_logger, "send protocol failed,debug string = {}",
                          msg->debugString());
    }
  }
}

void RpcClient::handleRecv() {
  Protocol::ptr msg;
  while (true) {
    msg = m_session->recvProtocol();
    if (msg == nullptr) {
      SPDLOG_LOGGER_ERROR(g_logger, "server closed");
      close();
      return;
    }
    uint8_t type = uint8_t(msg->getType());
    switch (type) {
      case (uint8_t)Protocol::Type::kRpcHeartbeat: handleHeartbeat(); break;
      case (uint8_t)Protocol::Type::kRpcMethodResponse:
        handleMethodResponse(msg);
        break;
      default: SPDLOG_LOGGER_WARN(g_logger, "unknown protocol"); break;
    }
  }
}

void RpcClient::handleHeartbeat() {
  SPDLOG_LOGGER_INFO(g_logger, "receive server heartbeat,server:{}",
                     m_session->getPeerAddressString());
  m_is_heartbeat_response = true;
}

void RpcClient::handleMethodResponse(Protocol::ptr msg) {
  uint64_t id = msg->getSeqId();
  if (m_outstanding_call.find(id) == m_outstanding_call.end()) {
    SPDLOG_LOGGER_WARN(g_logger, "unknown sequence id = {}", id);
    return;
  }
  SPDLOG_LOGGER_DEBUG(g_logger, "recv method call result,seq = {}", id);
  m_outstanding_call[id] << msg;
}

void RpcClient::close() {
  if (m_is_closed) {
    return;
  }
  m_is_closed = true;
  m_chan.Close();
  if (m_session && m_session->isConnected()) {
    m_session->close();
  }

  if (m_heartbeat_timer_id) {
    m_heartbeat_timer_id.StopTimer();
  }
}
} // namespace xzmjx::rpc