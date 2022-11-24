//
// Created by 20132 on 2022/11/23.
//
#include "spdlog/fmt/fmt.h"
using namespace fmt::literals;
int main() {
    fmt::print("{name}\n",fmt::arg("name","xzmjx"));
    fmt::print("{name}\n","name"_a="xzmjx");
    fmt::print("{1} and {1} and {0}\n","xzmjx","xx");
    char c = 120;
    fmt::print("{:6}\n", 42);    // value of s0 is "    42"
    fmt::print("{:6}\n", 'x');   // value of s1 is "x     "
    fmt::print("{:*<6}\n", 'x'); // value of s2 is "x*****"
    fmt::print("{:*>6}\n", 'x'); // value of s3 is "*****x"
    fmt::print("{:*^6}\n", 'x'); // value of s4 is "**x***"
    fmt::print("{:6d}\n", c);    // value of s5 is "   120"
    fmt::print("{:6}\n", true);  // value of s6 is "true  "
    double inf = std::numeric_limits<double>::infinity();
    double nan = std::numeric_limits<double>::quiet_NaN();
    fmt::print("{0:},{0:+},{0:-},{0: }\n", 1);   // value of s0 is "1,+1,1, 1"
    fmt::print("{0:},{0:+},{0:-},{0: }\n", -1);  // value of s1 is "-1,-1,-1,-1"
    fmt::print("{0:},{0:+},{0:-},{0: }\n", inf); // value of s2 is "inf,+inf,inf, inf"
    fmt::print("{0:},{0:+},{0:-},{0: }\n", nan); // value of s3 is "nan,+nan,nan, nan"
    return 0;
}