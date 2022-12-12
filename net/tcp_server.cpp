//
// Created by 20132 on 2022/11/29.
//

#include "tcp_server.h"

#include "utils/config.h"
#include "utils/log.h"

namespace xzmjx {
namespace {
std::shared_ptr<spdlog::logger> g_logger = log::GetDefault();
xzmjx::ConfigVar<uint64_t>::ptr g_tcp_server_read_timeout =
    xzmjx::Config::Lookup("tcp_server.read_timeout", (uint64_t)(60 * 1000 * 2),
                          "tcp server read timeout");
}  // namespace
TcpServer::TcpServer(co::Scheduler* worker, co::Scheduler* io_worker,
                     co::Scheduler* accept_worker)
    : m_worker(worker),
      m_io_worker(io_worker),
      m_accept_worker(accept_worker),
      m_recv_timeout(g_tcp_server_read_timeout->getValue()),
      m_name("xzmjx/1.0.0"),
      m_is_stop(true) {}

TcpServer::~TcpServer() {
  for (auto i : m_listen_socks) {
    i->close();
  }
  m_listen_socks.clear();
}

bool TcpServer::bind(Address::ptr addr) {
  std::vector<Address::ptr> addrs;
  std::vector<Address::ptr> fails;
  addrs.push_back(addr);
  return bind(addrs, fails);
}

bool TcpServer::bind(const std::vector<Address::ptr>& addrs,
                     std::vector<Address::ptr>& fails) {
  for (auto& addr : addrs) {
    Socket::ptr sock = Socket::CreateTCP(addr);
    if (!sock->bind(addr)) {
      SPDLOG_LOGGER_ERROR(g_logger, "bind failed errno={} errstr={} addr=[{}]",
                          errno, strerror(errno), addr->toString());
      fails.push_back(addr);
      continue;
    }
    if (!sock->listen()) {
      SPDLOG_LOGGER_ERROR(g_logger,
                          "listen failed errno={} errstr={} addr=[{}]", errno,
                          strerror(errno), addr->toString());
      fails.push_back(addr);
      continue;
    }
    m_listen_socks.push_back(sock);
  }
  if (!fails.empty()) {
    m_listen_socks.clear();
    return false;
  }
  for (auto& i : m_listen_socks) {
    SPDLOG_LOGGER_INFO(g_logger, "type={} name={} server bind success:{} ",
                       m_type, m_name, i->toString());
  }
  return true;
}

bool TcpServer::start() {
  if (!m_is_stop) {
    return true;  ///正在运行
  }
  m_is_stop = false;
  for (auto& sock : m_listen_socks) {
    ///!!!std::bind()支持绑定虚函数，即使写作TcpServer::startAccept,也会触发虚函数的override机制
    go co_scheduler(m_accept_worker)
        std::bind(&TcpServer::startAccept, shared_from_this(), sock);
  }

  if (m_worker != m_accept_worker) {
    std::thread t([this]() { this->m_worker->Start(0); });
    t.detach();
  }

  m_accept_worker->Start(0);
  return true;
}

void TcpServer::stop() {
  m_is_stop = true;
  auto self = shared_from_this();
  go co_scheduler(m_accept_worker)[this, self]() {
    for (auto& sock : m_listen_socks) {
      sock->close();
    }
    m_listen_socks.clear();
  };
}
std::string TcpServer::toString(const std::string& prefix) {
  std::stringstream ss;
  //    ss<<prefix<<"[type="<<m_type<<" name="<<m_name<<" worker="
  //      <<m_worker->getName()<<" io_worker="<<m_io_worker->getName()
  //      <<" accept_worker="<<m_accept_worker->getName()
  //      <<" recv_timeout="<<m_recv_timeout
  //      <<std::endl;

  std::string str = prefix.empty() ? "    " : prefix;
  for (auto& sock : m_listen_socks) {
    ss << str << sock->toString() << std::endl;
  }
  return ss.str();
}

void TcpServer::handleClient(Socket::ptr client) {
  SPDLOG_LOGGER_ERROR(g_logger, "handleClient: {}", client->toString());
}

void TcpServer::startAccept(Socket::ptr sock) {
  while (!m_is_stop) {
    Socket::ptr client = sock->accept();
    if (client) {
      client->setRecvTimeout(m_recv_timeout);
      go co_scheduler(m_io_worker)
          std::bind(&TcpServer::handleClient, shared_from_this(), client);
    } else {
      SPDLOG_LOGGER_ERROR(g_logger, "accept errno={} errstr={}", errno,
                          strerror(errno));
    }
  }
}
}  // namespace xzmjx
