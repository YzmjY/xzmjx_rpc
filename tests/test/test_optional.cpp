//
// Created by 20132 on 2022/11/27.
//
#include <optional>
#include <iostream>

class X{
private:
    int x;
};

std::optional<X> some_func_return_x(bool b) {
    if(b) {
        return X{};
    } else {
        return std::nullopt;
    }
};



int main() {
    if(some_func_return_x(false) != std::nullopt) {
        std::cout<<"failed"<<std::endl;
    }
    if(some_func_return_x(true) == std::nullopt) {
        std::cout<<"failed"<<std::endl;
    }
    return 0;
}