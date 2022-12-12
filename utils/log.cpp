//
// Created by 20132 on 2022/11/24.
//
#include "log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
namespace xzmjx {
namespace log {
static std::shared_ptr<spdlog::logger> GetLogInstanceHelper() {
  // auto instance = spdlog::stdout_color_mt("xzmjx");
  auto instance = spdlog::default_logger();
  instance->set_level(spdlog::level::trace);
  SPDLOG_LOGGER_INFO(instance, "ENABLE_DEBUGGER");
  return instance;
}

std::shared_ptr<spdlog::logger> GetDefault() {
  static std::shared_ptr<spdlog::logger> instance = GetLogInstanceHelper();
  return instance;
}
}  // namespace log

}  // namespace xzmjx