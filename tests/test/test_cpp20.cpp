//
// Created by 20132 on 2022/11/25.
//
#include "spdlog/fmt/fmt.h"
#include <ranges>
#include <string_view>
#include <iostream>

consteval int sqrt(int x) {
    return x*x;
}

consteval int sqrsqrt(int x) {
    return sqrt(sqrt(x));
}

template<typename T>
struct S {
    constexpr operator bool() const {return true;}
};

// 不允许隐式转换
template <typename T>
requires (boo(S<T>{}))
void foo(T);


void foo(int) {
    fmt::print("foo()\n");
    return ;
}


void constraint_auto(std::integral auto x) {
    fmt::print("{}\n",x);
}

using namespace std::literals;
#ifdef NONE
int main() {
    /*--------------------------Three-way comparison------------------------------*/
//    float a = 3.14;
//    double b = 2.56;
//    auto i = a<=>b;
//    if(i == std::partial_ordering::less)
//        fmt::print("a {} b\n","<");
//    else if (i == std::partial_ordering::greater)
//        fmt::print("a {} b\n",">");
//    else
//        fmt::print("a {} b\n","==");
//
//    int a1 = 3;
//    int b1 = 2;
//    auto i1 = a1<=>b1;
//    if(i1 == std::strong_ordering::less)
//        fmt::print("a {} b\n","<");
//    else if (i1 == std::strong_ordering::greater)
//        fmt::print("a {} b\n",">");
//    else
//        fmt::print("a {} b\n","==");

    /*--------------------------Range------------------------------*/
//    //constrained algorithms
//    std::vector<int> vec = {2,8,4,5,3,1};
//    std::ranges::sort(vec);
//    for(auto&& i : vec) {
//        fmt::print("{} ",i);
//    }
//    fmt::print("\n");
//
//    // 范围和范围适配器
//    std::vector<int> vec1{ 20,1,12,4,20,3,10,1 };
//
//    auto even = [](const int& a)
//    {
//        return a % 2 == 0;
//    };
//
//    auto square = [](const int& a) {return a * a; };
//    for(int i: vec1
//                | std::views::filter(even)
//                | std::views::transform(square)
//                | std::views::take(2)) {
//        fmt::print("{} ",i);
//    }
//    fmt::print("\n");
//
//    std::vector<std::vector<int>> vec2 = {{1,2},{3,4},{5,6}};
//    for(int i : vec2
//                | std::views::join) {
//        fmt::print("{} ",i);
//    }
//    fmt::print("\n");
//
//    auto parts={"c"sv,"+"sv,"+"sv,"20"sv};
//    for(auto s : parts | std::views::join) {
//        fmt::print("{}",s);
//    }
//    fmt::print("\n");
//
//    std::vector<std::tuple<int,int,int>> vec3 = {{1,2,3},{4,5,6},{7,8,9}};
//    for(auto e: vec3
//                | std::views::elements<1>) {
//        fmt::print("{} ",e);
//    }
//    fmt::print("\n");

    /*--------------------------likely/unlikely------------------------------*/
//    int n = 2;
//    switch (n) {
//    case 1:
//        fmt::print("first case\n");
//            break;
//    [[likely]] case 2:
//        fmt::print("second case\n");
//    }
//
//    if(n != 2)[[unlikely]] {
//        fmt::print("\n");
//    } else {
//        fmt::print("if likely\n");
//    }
    /*--------------------------consteval------------------------------*/
//    int n = 0;
//    sqrt(n);
//    constexpr int n = 4;
//    static_assert(sqrt(n)==16);
//    fmt::print("{}\n",sqrt(n));
//    fmt::print("{}\n",sqrsqrt(n));

    /*--------------------------constraint auto func------------------------------*/
    //constraint_auto("x");
    //foo(1);
    return 0;
}
#endif

#include <iostream>
#include <type_traits>

#define OUT(...) std::cout << #__VA_ARGS__ << " : " << __VA_ARGS__ << '\n'

class A {};

int main()
{
    std::cout << std::boolalpha;
    OUT( std::is_bounded_array_v<A> );
    OUT( std::is_bounded_array_v<A[]> );
    OUT( std::is_bounded_array_v<A[3]> );
    OUT( std::is_bounded_array_v<float> );
    OUT( std::is_bounded_array_v<int> );
    OUT( std::is_bounded_array_v<int[]> );
    OUT( std::is_bounded_array_v<int[3]> );
}