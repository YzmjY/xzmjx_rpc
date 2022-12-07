//
// Created by 20132 on 2022/11/26.
//

#ifndef XZMJX_TIMER_MEASURE_H
#define XZMJX_TIMER_MEASURE_H
#include <string>
#include <chrono>
#include "spdlog/fmt/fmt.h"
using namespace fmt::literals;
class TimerWrapper {
private:
    std::string m_name;
    std::chrono::high_resolution_clock::time_point m_start_time;

public:
    TimerWrapper(const std::string& name) : m_name(name) {
        m_start_time = std::chrono::high_resolution_clock::now();
    }

    ~TimerWrapper() {
        TimeCost();
    }

    void TimeCost() {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_start_time);
        fmt::print("{name} test costs {time}","name"_a=m_name,"time"_a=ms.count());
    }
};
#endif //XZMJX_TIMER_MEASURE_H
