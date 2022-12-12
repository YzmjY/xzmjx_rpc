//
// Created by 20132 on 2022/11/24.
//

#ifndef XZMJX_LOG_H
#define XZMJX_LOG_H
#include <memory>

#include "libgo.h"
#include "spdlog/spdlog.h"
namespace xzmjx {
namespace log {
std::shared_ptr<spdlog::logger> GetDefault();
}
}  // namespace xzmjx

#endif  // XZMJX_LOG_H
