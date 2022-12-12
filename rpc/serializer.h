//
// Created by 20132 on 2022/11/30.
//

#ifndef XZMJX_SERIALIZER_H
#define XZMJX_SERIALIZER_H
#include <list>
#include <map>
#include <set>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "utils/byte_array.h"
namespace xzmjx::rpc {
class Serializer {
 public:
  using ptr = std::shared_ptr<Serializer>;

  Serializer() { m_bt = std::make_shared<ByteArray>(); }

  explicit Serializer(const std::string& data) {
    m_bt = std::make_shared<ByteArray>();
    m_bt->write(data.c_str(), data.size());
    m_bt->setPosition(0);
  }

  explicit Serializer(ByteArray::ptr bt) { m_bt = bt; }

  void skip(std::size_t cnt) {
    std::size_t cur = m_bt->getPosition();
    m_bt->setPosition(cur + cnt);
  }

  void reset() { m_bt->setPosition(0); }

  void clear() { m_bt->clear(); }

  std::string toString() const { return m_bt->toString(); }

  /**
   * 基本类型的序列化
   * @tparam T
   * @param v
   */
  template <class T>
  [[maybe_unused]] void serialize(const T& v) {
    if constexpr (std::is_same_v<T, bool>) {
      m_bt->writeFint8(v);
    } else if constexpr (std::is_same_v<T, float>) {
      m_bt->writeFloat(v);
    } else if constexpr (std::is_same_v<T, double>) {
      m_bt->writeDouble(v);
    } else if constexpr (std::is_same_v<T, uint8_t>) {
      m_bt->writeFuint8(v);
    } else if constexpr (std::is_same_v<T, int8_t>) {
      m_bt->writeFint8(v);
    } else if constexpr (std::is_same_v<T, uint16_t>) {
      m_bt->writeFuint16(v);
    } else if constexpr (std::is_same_v<T, int16_t>) {
      m_bt->writeFint16(v);
    } else if constexpr (std::is_same_v<T, uint32_t>) {
      m_bt->writeUint32(v);
    } else if constexpr (std::is_same_v<T, int32_t>) {
      m_bt->writeInt32(v);
    } else if constexpr (std::is_same_v<T, uint64_t>) {
      m_bt->writeUint64(v);
    } else if constexpr (std::is_same_v<T, int64_t>) {
      m_bt->writeInt64(v);
    } else if constexpr (std::is_same_v<T, std::string>) {
      m_bt->writeStringVint(v);
    } else if constexpr (std::is_same_v<T, char*>) {
      m_bt->writeStringVint(std::string(v));
    } else if constexpr (std::is_same_v<T, const char*>) {
      m_bt->writeStringVint(std::string(v));
    }
  }

  /**
   * 基本类型的反序列化
   * @tparam T
   * @param v
   */
  template <class T>
  [[maybe_unused]] void deserialize(T& v) {
    if constexpr (std::is_same_v<T, bool>) {
      v = m_bt->readFint8();
    } else if constexpr (std::is_same_v<T, float>) {
      v = m_bt->readFloat();
    } else if constexpr (std::is_same_v<T, double>) {
      v = m_bt->readDouble();
    } else if constexpr (std::is_same_v<T, uint8_t>) {
      v = m_bt->readFuint8();
    } else if constexpr (std::is_same_v<T, int8_t>) {
      v = m_bt->readFint8();
    } else if constexpr (std::is_same_v<T, uint16_t>) {
      v = m_bt->readFuint16();
    } else if constexpr (std::is_same_v<T, int16_t>) {
      v = m_bt->readFint16();
    } else if constexpr (std::is_same_v<T, uint32_t>) {
      v = m_bt->readUint32();
    } else if constexpr (std::is_same_v<T, int32_t>) {
      v = m_bt->readInt32();
    } else if constexpr (std::is_same_v<T, uint64_t>) {
      v = m_bt->readUint64();
    } else if constexpr (std::is_same_v<T, int64_t>) {
      v = m_bt->readInt64();
    } else if constexpr (std::is_same_v<T, std::string>) {
      v = m_bt->readStringVint();
    }
  }

  template <class T>
  [[maybe_unused]] Serializer& operator<<(const T& data) {
    serialize(data);
    return *this;
  }

  template <class T>
  [[maybe_unused]] Serializer& operator>>(T& data) {
    deserialize(data);
    return *this;
  }

  /*
   * 支持的stl容器vector,list,set,map,unordered_set,unordered_map,tuple
   */
  template <class T>
  [[maybe_unused]] Serializer& operator<<(const std::vector<T>& data) {
    (*this) << (data.size());
    for (auto&& it : data) {
      (*this) << it;
    }
    return *this;
  }

  template <class T>
  [[maybe_unused]] Serializer& operator>>(std::vector<T>& data) {
    std::size_t n;
    (*this) >> (n);
    for (size_t i = 0; i < n; i++) {
      T v{};
      (*this) >> v;
      data.push_back(v);
    }
    return *this;
  }

  template <class T>
  [[maybe_unused]] Serializer& operator<<(const std::list<T>& data) {
    (*this) << (data.size());
    for (auto&& it : data) {
      (*this) << it;
    }
    return *this;
  }

  template <class T>
  [[maybe_unused]] Serializer& operator>>(std::list<T>& data) {
    std::size_t n;
    (*this) >> (n);
    for (size_t i = 0; i < n; i++) {
      T v{};
      (*this) >> v;
      data.push_back(v);
    }
    return *this;
  }

  template <class T>
  [[maybe_unused]] Serializer& operator<<(const std::set<T>& data) {
    (*this) << (data.size());
    for (auto&& it : data) {
      (*this) << it;
    }
    return *this;
  }

  template <class T>
  [[maybe_unused]] Serializer& operator>>(std::set<T>& data) {
    std::size_t n;
    (*this) >> n;
    for (size_t i = 0; i < n; i++) {
      T v{};
      (*this) >> v;
      data.insert(v);
    }
    return *this;
  }

  template <class K, class V>
  [[maybe_unused]] Serializer& operator<<(const std::map<K, V>& data) {
    (*this) << (data.size());
    for (auto iter = data.begin(); iter != data.end(); iter++) {
      this << iter->first << iter->second;
    }
    return *this;
  }

  template <class K, class V>
  [[maybe_unused]] Serializer& operator>>(std::map<K, V>& data) {
    std::size_t n;
    (*this) >> n;
    for (size_t i = 0; i < n; i++) {
      std::pair<K, V> v{};
      (*this) >> (v.first);
      (*this) >> (v.second);
      data[v.first] = v.second;
    }
    return *this;
  }

  template <class K, class V>
  [[maybe_unused]] Serializer& operator<<(const std::pair<K, V>& data) {
    this << data->first << data->second;
    return *this;
  }

  template <class K, class V>
  [[maybe_unused]] Serializer& operator>>(std::pair<K, V>& data) {
    (*this) >> (data.first);
    (*this) >> (data.second);
    return *this;
  }

  template <class T>
  [[maybe_unused]] Serializer& operator<<(const std::unordered_set<T>& data) {
    (*this) << (data.size());
    for (auto&& it : data) {
      (*this) << it;
    }
    return *this;
  }

  template <class T>
  [[maybe_unused]] Serializer& operator>>(std::unordered_set<T>& data) {
    std::size_t n;
    (*this) >> (n);
    for (size_t i = 0; i < n; i++) {
      T v{};
      (*this) >> (v);
      data.insert(v);
    }
    return *this;
  }

  template <class K, class V>
  [[maybe_unused]] Serializer& operator<<(
      const std::unordered_map<K, V>& data) {
    (*this) << (data.size());
    for (auto iter = data.begin(); iter != data.end(); iter++) {
      this << iter->first << iter->second;
    }
    return *this;
  }

  template <class K, class V>
  [[maybe_unused]] Serializer& operator>>(std::unordered_map<K, V>& data) {
    std::size_t n;
    (*this) >> (n);
    for (size_t i = 0; i < n; i++) {
      std::pair<K, V> v{};
      (*this) >> (v.first);
      (*this) >> (v.second);
      data[v.first] = v.second;
    }
    return *this;
  }

  template <class... Types>
  [[maybe_unused]] Serializer& operator<<(const std::tuple<Types...>& tp) {
    auto package = [this]<class TupleType, std::size_t... Index>(
        const TupleType& t, std::index_sequence<Index...> index) {
      ((*this) << ... << std::get<Index>(t));
    };
    package(tp, std::index_sequence_for<Types...>{});
    return *this;
  }

  template <class... Types>
  [[maybe_unused]] Serializer& operator>>(std::tuple<Types...>& tp) {
    auto package = [this]<class TupleType, std::size_t... Index>(
        TupleType & t, std::index_sequence<Index...> index) {
      ((*this) >> ... >> std::get<Index>(t));
    };
    package(tp, std::index_sequence_for<Types...>{});
    return *this;
  }

 private:
  ByteArray::ptr m_bt;
};
}  // namespace xzmjx::rpc

#endif  // XZMJX_SERIALIZER_H
