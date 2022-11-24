//
// Created by 20132 on 2022/11/24.
//

#ifndef XZMJX_LOG_H
#define XZMJX_LOG_H
#include "spdlog/spdlog.h"
#include "libgo.h"
#include <memory>
namespace xzmjx {
namespace log {
std::shared_ptr<spdlog::logger> GetDefault();
}
}

#endif //XZMJX_LOG_H
