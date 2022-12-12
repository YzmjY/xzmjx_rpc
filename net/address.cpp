//
// Created by 20132 on 2022/11/29.
//
#include "address.h"

#include <netdb.h>
#include <stddef.h>

#include "utils/endian_cast.h"
#include "utils/log.h"
namespace xzmjx {
namespace {
std::shared_ptr<spdlog::logger> g_logger = log::GetDefault();
}

template <typename T>
static T CreateMask(uint32_t bits) {
  return (1 << (sizeof(T) * 8 - bits)) - 1;
}
Address::ptr Address::Create(const sockaddr *addr, socklen_t addrlen) {
  if (addr == nullptr) {
    return nullptr;
  }
  Address::ptr result;
  switch (addr->sa_family) {
    case AF_INET:
      result.reset(
          new IPv4Address(*(reinterpret_cast<const sockaddr_in *>(addr))));
      break;
    case AF_INET6:
      result.reset(
          new IPv6Address(*(reinterpret_cast<const sockaddr_in6 *>(addr))));
      break;
    default:
      result.reset(new UnknownAddress(*addr));
      break;
  }
  return result;
}

bool Address::Lookup(std::vector<Address::ptr> &result, const std::string &host,
                     int family, int type, int protocol) {
  addrinfo hint, *results, *next;
  hint.ai_flags = 0;
  hint.ai_family = family;
  hint.ai_socktype = type;
  hint.ai_protocol = protocol;
  hint.ai_addrlen = 0;
  hint.ai_canonname = NULL;
  hint.ai_addr = NULL;
  hint.ai_next = NULL;

  std::string node;
  const char *service = NULL;

  /// ipv6 [xx:xx:xx:xx::xx::xx::xx::xx]:port
  if (!host.empty() && host[0] == '[') {
    const char *endipv6 =
        (const char *)memchr(host.c_str() + 1, ']', host.size() - 1);
    if (endipv6) {
      if (*(endipv6 + 1) == ':') {
        service = endipv6 + 2;
      }
      node = host.substr(1, endipv6 - host.c_str() - 1);
    }
  }

  ///对于如下情况：www.baidu.com:80
  if (node.empty()) {
    service = (const char *)memchr(host.c_str(), ':', host.size());
    if (service) {
      if (!memchr(service + 1, ':', host.c_str() + host.size() - service - 1)) {
        node = host.substr(0, service - host.c_str());
        ++service;
      }
    }
  }

  if (node.empty()) {
    node = host;
  }
  int error = getaddrinfo(node.c_str(), service, &hint, &results);
  if (error) {
    SPDLOG_LOGGER_ERROR(g_logger,
                        "Address::Lookup getaddress({},{},{}) err={} errstr={}",
                        host, family, type, error, gai_strerror(error));
    return false;
  }
  next = results;
  while (next) {
    result.push_back(Create(next->ai_addr, (socklen_t)next->ai_addrlen));
    next = next->ai_next;
  }
  freeaddrinfo(results);
  return !result.empty();
}

Address::ptr Address::LookupAny(const std::string &host, int family, int type,
                                int protocol) {
  std::vector<Address::ptr> result;
  if (Lookup(result, host, family, type, protocol)) {
    return result[0];
  }
  return nullptr;
}

std::shared_ptr<IPAddress> Address::LookupAnyIPAddress(const std::string &host,
                                                       int family, int type,
                                                       int protocol) {
  std::vector<Address::ptr> result;
  if (Lookup(result, host, family, type, protocol)) {
    for (auto it : result) {
      IPAddress::ptr v = std::dynamic_pointer_cast<IPAddress>(it);
      if (v) {
        return v;
      }
    }
  }
  return nullptr;
}

///@TODO:网卡
bool Address::GetInterfaceAddress(
    std::multimap<std::string, std::pair<Address::ptr, uint32_t>> &result,
    int family) {
  return false;
}

bool Address::GetInterfaceAddress(std::pair<Address::ptr, uint32_t> &result,
                                  const std::string &iface, int family) {
  return false;
}

int Address::getFamily() const { return getAddr()->sa_family; }

std::string Address::toString() {
  std::stringstream ss;
  insert(ss);
  return ss.str();
}

bool Address::operator<(const Address &rhs) const {
  int min_size = std::min(getAddrLen(), rhs.getAddrLen());
  int result = memcmp(getAddr(), rhs.getAddr(), min_size);
  if (result < 0) {
    return true;
  } else if (result > 0) {
    return false;
  } else if (getAddrLen() < getAddrLen()) {
    return true;
  }
  return false;
}

bool Address::operator==(const Address &rhs) const {
  return getAddrLen() == rhs.getAddrLen() &&
         memcmp(getAddr(), rhs.getAddr(), getAddrLen()) == 0;
}

bool Address::operator!=(const Address &rhs) const { return !(*this == rhs); }

IPAddress::ptr IPAddress::Create(const char *address, uint16_t port) {
  addrinfo hints, *results;
  memset(&hints, 0, sizeof(addrinfo));

  hints.ai_flags = AI_NUMERICHOST;
  hints.ai_family = AF_UNSPEC;

  int error = getaddrinfo(address, NULL, &hints, &results);
  if (error) {
    SPDLOG_LOGGER_ERROR(g_logger,
                        "IPAdress::Create({},{}) error={} errno={} errstr={}",
                        address, port, error, errno, strerror(errno));
  }
  try {
    IPAddress::ptr ans = std::dynamic_pointer_cast<IPAddress>(
        Address::Create(results->ai_addr, (socklen_t)results->ai_addrlen));
    if (ans) {
      ans->setPort(port);
    }
    freeaddrinfo(results);
    return ans;
  } catch (...) {
    freeaddrinfo(results);
    return nullptr;
  }
}

IPv4Address::ptr IPv4Address::Create(const char *address, uint16_t port) {
  IPv4Address::ptr rt(new IPv4Address);
  rt->m_address.sin_port = EndianCastByType(port, std::endian::big);
  int result = inet_pton(AF_INET, address, &rt->m_address.sin_addr);
  if (result <= 0) {
    SPDLOG_LOGGER_ERROR(g_logger,
                        "IPv4Address::Create({},{}) rt={} errno={} errstr={}",
                        address, port, result, errno, strerror(errno));
    return nullptr;
  }
  return rt;
}

IPv4Address::IPv4Address(const sockaddr_in &address) { m_address = address; }

IPv4Address::IPv4Address(uint32_t address, uint16_t port) {
  m_address.sin_addr.s_addr = EndianCastByType(address, std::endian::big);
  m_address.sin_family = AF_INET;
  m_address.sin_port = EndianCastByType(port, std::endian::big);
}

const sockaddr *IPv4Address::getAddr() const { return (sockaddr *)&m_address; }

sockaddr *IPv4Address::getAddr() { return (sockaddr *)&m_address; }

socklen_t IPv4Address::getAddrLen() const { return sizeof(m_address); }

IPAddress::ptr IPv4Address::broadcastAddress(uint32_t prefix_len) {
  if (prefix_len > 32) {
    ///@details ipv4地址只有32位;
    return nullptr;
  }
  uint32_t ipv4_mask =
      CreateMask<uint32_t>(prefix_len);  ///创建32-prefix_len（子网掩码）个1
  sockaddr_in copy = m_address;
  copy.sin_addr.s_addr |= EndianCastByType(ipv4_mask, std::endian::big);
  IPv4Address::ptr ans = std::make_shared<IPv4Address>(copy);
  return ans;
}

IPAddress::ptr IPv4Address::networkAddress(uint32_t prefix_len) {
  if (prefix_len > 32) {
    ///@details ipv4地址只有32位;
    return nullptr;
  }
  uint32_t ipv4_mask = CreateMask<uint32_t>(prefix_len);
  sockaddr_in copy = m_address;
  copy.sin_addr.s_addr &=
      ~EndianCastByType(ipv4_mask, std::endian::big);  ///@TODO 待测试
  IPv4Address::ptr ans = std::make_shared<IPv4Address>(copy);
  return ans;
}

IPAddress::ptr IPv4Address::subnetMask(uint32_t prefix_len) {
  sockaddr_in subnet;
  memset(&subnet, 0, sizeof(subnet));
  subnet.sin_addr.s_addr =
      ~EndianCastByType(CreateMask<uint32_t>(prefix_len), std::endian::big);
  subnet.sin_family = AF_INET;
  IPv4Address::ptr ans = std::make_shared<IPv4Address>(subnet);
  return ans;
}

std::ostream &IPv4Address::insert(std::ostream &out) {
  /// socketaddr里保存的是网络字节序，转换为主机字节序
  uint32_t addr = EndianCastByType(m_address.sin_addr.s_addr, std::endian::big);
  out << ((addr >> 24) & 0xff) << ":" << ((addr >> 16) & 0xff) << ":"
      << ((addr >> 8) & 0xff) << ":" << (addr & 0xff);
  out << ":" << EndianCastByType(m_address.sin_port, std::endian::big);
  return out;
}

uint32_t IPv4Address::getPort() const {
  return EndianCastByType(m_address.sin_port, std::endian::big);
}

void IPv4Address::setPort(uint16_t port) {
  m_address.sin_port = EndianCastByType(port, std::endian::big);
}

IPv6Address::ptr IPv6Address::Create(const char *address, uint16_t port) {
  IPv6Address::ptr ans = std::make_shared<IPv6Address>();
  ans->m_address.sin6_port = EndianCastByType(port, std::endian::big);
  ans->m_address.sin6_family = AF_INET6;
  int error = inet_pton(AF_INET6, address, &ans->m_address.sin6_addr);
  if (error <= 0) {
    SPDLOG_LOGGER_ERROR(g_logger,
                        "IPv4Address::Create({},{}) rt={} errno={} errstr={}",
                        address, port, error, errno, strerror(errno));
    return nullptr;
  }
  return ans;
}

IPv6Address::IPv6Address(const sockaddr_in6 &address) { m_address = address; }
IPv6Address::IPv6Address() {
  memset(&m_address, 0, sizeof(m_address));
  m_address.sin6_family = AF_INET6;
}
IPv6Address::IPv6Address(const uint8_t address[16], uint16_t port) {
  m_address.sin6_port = EndianCastByType(port, std::endian::big);
  m_address.sin6_family = AF_INET6;
  ///@TODO： 这里已经是网络字节序了？怎么与IPv4的不一致
  /// ipv6地址是一组字节序列，没有大小端的概念
  memcpy(&m_address.sin6_addr.s6_addr, address, 16);
}

const sockaddr *IPv6Address::getAddr() const { return (sockaddr *)&m_address; }

sockaddr *IPv6Address::getAddr() { return (sockaddr *)&m_address; }

socklen_t IPv6Address::getAddrLen() const { return sizeof(m_address); }

std::ostream &IPv6Address::insert(std::ostream &out) {
  out << "[";
  uint16_t *addr = (uint16_t *)m_address.sin6_addr.s6_addr;
  bool used_zero = false;
  for (size_t i = 0; i < 8; i++) {
    ///@details
    ///IPv6地址中一连串的0可以忽略，例如2001:0000:3238:00E1:0063:0000:0000:FEFB->2001:0:3238:E1:0063::FEFB
    /// 但只能使用一次不然会造成歧义
    if (addr[i] == 0 && !used_zero) {
      continue;
    }
    if (i && addr[i - 1] == 0 && !used_zero) {
      out << ":";
      used_zero = true;
    }
    if (i) {
      out << ":";
    }
    ///@details 十六进制格式输出，输出完成后恢复流为十进制
    out << std::hex << (int)EndianCastByType(addr[i], std::endian::big)
        << std::dec;
  }
  if (!used_zero && addr[7] == 0) {
    out << "::";
  }
  out << "]:" << EndianCastByType(m_address.sin6_port, std::endian::big);
  return out;
}

IPAddress::ptr IPv6Address::broadcastAddress(uint32_t prefix_len) {
  sockaddr_in6 baddr = m_address;
  baddr.sin6_addr.s6_addr[prefix_len / 8] |=
      CreateMask<uint8_t>(prefix_len % 8);
  for (int i = prefix_len / 8 + 1; i < 16; i++) {
    baddr.sin6_addr.s6_addr[i] = 0xff;
  }
  IPv6Address::ptr ans = std::make_shared<IPv6Address>(baddr);
  return ans;
}

IPAddress::ptr IPv6Address::networkAddress(uint32_t prefix_len) {
  sockaddr_in6 baddr = m_address;
  baddr.sin6_addr.s6_addr[prefix_len / 8] &=
      ~CreateMask<uint8_t>(prefix_len % 8);
  for (int i = prefix_len / 8 + 1; i < 16; i++) {
    baddr.sin6_addr.s6_addr[i] = 0x00;
  }
  IPv6Address::ptr ans = std::make_shared<IPv6Address>(baddr);
  return ans;
}

IPAddress::ptr IPv6Address::subnetMask(uint32_t prefix_len) {
  sockaddr_in6 subnet;
  memset(&subnet, 0, sizeof(subnet));
  subnet.sin6_family = AF_INET6;
  subnet.sin6_addr.s6_addr[prefix_len / 8] =
      ~CreateMask<uint8_t>(prefix_len % 8);
  for (uint32_t i = 0; i < prefix_len / 8; i++) {
    subnet.sin6_addr.s6_addr[i] = 0xff;
  }
  return IPv6Address::ptr(new IPv6Address(subnet));
}

uint32_t IPv6Address::getPort() const {
  return EndianCastByType(m_address.sin6_port, std::endian::big);
}

void IPv6Address::setPort(uint16_t port) {
  m_address.sin6_port = EndianCastByType(port, std::endian::big);
}

///@TODO: 减去/0的位置？
static const size_t MAX_PATH_LEN = sizeof(((sockaddr_un *)0)->sun_path) - 1;
UnixAddress::UnixAddress() {
  memset(&m_address, 0, sizeof(m_address));
  m_address.sun_family = AF_UNIX;
  m_length = offsetof(sockaddr_un, sun_path) + MAX_PATH_LEN;
}

UnixAddress::UnixAddress(const std::string &path) {
  memset(&m_address, 0, sizeof(m_address));
  m_length = path.size() + 1;

  ///@details: 空字符串但是空字符串的size()==0不是成立的吗
  if (!path.empty() && path[0] == '\0') {
    --m_length;
  }

  if (m_length > sizeof(m_address.sun_path)) {
    throw std::logic_error("path too long");
  }
  memcpy(m_address.sun_path, path.c_str(), m_length);
  m_length += offsetof(sockaddr_un, sun_path);
}

UnixAddress::~UnixAddress() {}

const sockaddr *UnixAddress::getAddr() const { return (sockaddr *)&m_address; }

sockaddr *UnixAddress::getAddr() { return (sockaddr *)&m_address; }

socklen_t UnixAddress::getAddrLen() const { return m_length; }

void UnixAddress::setAddrLen(uint32_t v) { m_length = v; }

std::string UnixAddress::getPath() const {
  std::stringstream ss;
  if (m_length > offsetof(sockaddr_un, sun_path) &&
      m_address.sun_path[0] == '\0') {
    ss << "\\0"
       << std::string(m_address.sun_path + 1,
                      m_length - offsetof(sockaddr_un, sun_path) - 1);
  } else {
    ss << m_address.sun_path;
  }
  return ss.str();
}

std::ostream &UnixAddress::insert(std::ostream &out) {
  if (m_length > offsetof(sockaddr_un, sun_path) &&
      m_address.sun_path[0] == '\0') {
    return out << "\\0"
               << std::string(m_address.sun_path + 1,
                              m_length - offsetof(sockaddr_un, sun_path) - 1);
  }
  return out << m_address.sun_path;
}

UnknownAddress::UnknownAddress(int family) {
  memset(&m_address, 0, sizeof(m_address));
  m_address.sa_family = family;
}

UnknownAddress::UnknownAddress(const sockaddr &addr) { m_address = addr; }

UnknownAddress::~UnknownAddress() {}

const sockaddr *UnknownAddress::getAddr() const {
  return (sockaddr *)&m_address;
}

sockaddr *UnknownAddress::getAddr() { return (sockaddr *)&m_address; }

socklen_t UnknownAddress::getAddrLen() const { return sizeof(m_address); }

std::ostream &UnknownAddress::insert(std::ostream &out) {
  out << "UnknownAddress family = " << m_address.sa_family << "]";
  return out;
}

}  // namespace xzmjx
