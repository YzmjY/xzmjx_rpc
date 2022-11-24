//
// Created by 20132 on 2022/11/24.
//
#include "libgo.h"
#include "spdlog/fmt/fmt.h"
int cnt = 0;
int main() {
    co::co_mutex m;
    go [&](){
        m.lock();
        while(cnt<=100) {
            if(cnt%2) {
                m.unlock();
                co_yield;
            } else {
                fmt::print("co1:{:d}\n",cnt);
                cnt++;
            }
        }
    };

    go [&](){
        m.lock();
        while(cnt<=100) {
            if(cnt%2 == false) {
                m.unlock();
                co_yield;
            } else {
                fmt::print("co2:{:d}\n",cnt);
                cnt++;
            }
        }
    };

    co_timer timer(std::chrono::milliseconds(1), &co_sched);
    timer.ExpireAt(std::chrono::seconds(1), []{
        co_sched.Stop();
    });

    g_Scheduler.Start(1);
    return 0;
}
