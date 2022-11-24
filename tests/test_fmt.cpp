//
// Created by 20132 on 2022/11/23.
//
#include "spdlog/fmt/fmt.h"
using namespace fmt::literals;
int main() {
    fmt::print("{name}\n",fmt::arg("name","xzmjx"));
    fmt::print("{name}\n","name"_a="xzmjx");
    fmt::print("{1} and {1} and {0}\n","xzmjx","xx");
    return 0;
}