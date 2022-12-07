//
// Created by 20132 on 2022/11/24.
//
#include "libgo.h"
#include "utils/log.h"
#include "spdlog/fmt/fmt.h"
int cnt = 0;
std::function<void(void)> heartbeat_cb;
co_timer timer(std::chrono::milliseconds(1), &co_sched);
co_timer_id id;
int main() {
//    co::co_mutex m;
//    go [&](){
//        m.lock();
//        while(cnt<=100) {
//            if(cnt%2) {
//                m.unlock();
//                co_yield;
//            } else {
//                fmt::print("co1:{:d}\n",cnt);
//                cnt++;
//            }
//        }
//    };
//
//    go [&](){
//        m.lock();
//        while(cnt<=100) {
//            if(cnt%2 == false) {
//                m.unlock();
//                co_yield;
//            } else {
//                fmt::print("co2:{:d}\n",cnt);
//                cnt++;
//            }
//        }
//    };


    heartbeat_cb = []() {
        SPDLOG_LOGGER_INFO(xzmjx::log::GetDefault(),"heartbeat");
        id = timer.ExpireAt(std::chrono::seconds(5),heartbeat_cb);
    };
    id = timer.ExpireAt(std::chrono::seconds(5),heartbeat_cb);

    g_Scheduler.Start(1);
    return 0;
}
