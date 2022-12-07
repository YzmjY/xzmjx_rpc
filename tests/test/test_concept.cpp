//
// Created by 20132 on 2022/11/25.
//
#include <concepts>
#include <string>
#include <iostream>
#include "spdlog/fmt/fmt.h"

template <typename T>
concept diviable_3 = requires (T a, T b) {
    a/3;
};

template <typename T>
requires diviable_3<T>
class DivideImpl {
public:
    DivideImpl() {

    }
public:
    T Div(T a, T b) {
        return a/b;
    }

};

template <typename T>
concept Addable = requires (T a ,T b) {
    a+b;
};

struct foo {
    int foo;
};

struct bar {
    using value = int;
    value data;
};

struct baz {
    using value = int;
    value data;
};

template <typename T, typename = std::enable_if_t<std::is_same_v<T,baz>>>
struct S{};

template <typename T>
using Ref = T&;

template <typename T>
concept C = requires {
    typename T::value;
    typename S<T>;
    typename Ref<T>;
};

template<typename T>
requires C<T>
void g(T) {
    fmt::print("success");
}

template <typename T>
concept D = requires(T x) {
    {x + 1} -> std::same_as<int>;
    {x * 1} -> std::same_as<int>;
};

void h(D auto x) {
    fmt::print("{}",x);
}
int main() {
    DivideImpl<int> impl;
    std::cout<<impl.Div(2,3)<<std::endl;

    //g(bar{});
    //g(baz{});
    //g(foo{});
    // DivideImpl<std::string> err_impl;

    h(1);
    //h("x");
    return 0;
}