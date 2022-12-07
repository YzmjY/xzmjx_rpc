//
// Created by 20132 on 2022/11/29.
//
#include <iostream>


std::ostream& operator "" _x(const char*,std::size_t ) {
    return std::cout;
}

int main() {
    "输出"_x <<"xzmjx"<<std::endl;
}