//
// Created by 20132 on 2022/11/24.
//
#include "log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace xzmjx {
namespace log{
std::shared_ptr<spdlog::logger> GetDefault() {
    return spdlog::stdout_color_mt("xzmjx");
}
}

}