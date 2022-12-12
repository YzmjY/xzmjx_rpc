//
// Created by 20132 on 2022/11/23.
//

#ifndef XZMJX_ENDIAN_CAST_H
#define XZMJX_ENDIAN_CAST_H
#include <byteswap.h>
#include <stdint.h>

#include <bit>
#include <concepts>

namespace xzmjx {

template <typename T>
requires std::integral<T> constexpr T ByteSwap(T v) {
  if constexpr (sizeof(T) == sizeof(uint8_t)) {
    return v;
  } else if constexpr (sizeof(T) == sizeof(uint16_t)) {
    return (T)bswap_16((uint16_t)v);
  } else if constexpr (sizeof(T) == sizeof(uint32_t)) {
    return (T)bswap_32((uint32_t)v);
  } else if constexpr (sizeof(T) == sizeof(uint64_t)) {
    return (T)bswap_64((uint64_t)v);
  }
}

template <typename T>
requires std::integral<T> T EndianCastByType(T v, std::endian e) {
  if (std::endian::native == e) {
    return v;
  } else {
    return ByteSwap(v);
  }
}

}  // namespace xzmjx

#endif  // XZMJX_ENDIAN_CAST_H
