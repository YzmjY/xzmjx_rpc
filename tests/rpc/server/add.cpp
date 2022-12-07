//
// Created by 20132 on 2022/12/2.
//

#include "add.h"
#include "utils/log.h"
namespace xzmjx::test{
static auto logger = log::GetDefault();
uint32_t add(uint32_t a,uint32_t b) {
    SPDLOG_LOGGER_DEBUG(logger,"call add,a = {},b = {}",a,b);
    return a+b;
}

void say_hello(const std::string& s) {
    SPDLOG_LOGGER_DEBUG(logger,"hello {}",s);
}
}