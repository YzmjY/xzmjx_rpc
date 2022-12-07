//
// Created by 20132 on 2022/12/1.
//
#include <type_traits>
#include <iostream>
#include <tuple>
#include <cxxabi.h>
#include "utils/function_traits.h"
using namespace std;
template<typename Tp>
struct my_is_function
        : public __bool_constant<!is_const<const Tp>::value> { };

template<typename Tp>
struct my_is_function<Tp&>
        : public false_type { };

template<typename Tp>
struct my_is_function<Tp&&>
        : public false_type { };

void func(int i) {

}



struct X {
    int a;
    int test(int);
};

template <class>
struct PM_traits {};

template <class T, class U>
        struct PM_traits<U T::*> {
            using type = U;
            using type1=T;
        };

//template <class>
//struct function_traits{
//    inline static constexpr bool value = false;
//};
//
//template <class Ret, class ClassType,class ... Args>
//struct function_traits<Ret(ClassType::*)(Args...)> {
//    using ret_type = Ret;
//    using class_type = ClassType;
//    using tuple_type = std::tuple<Args...>;
//    inline static constexpr bool value = true;
//};


template <class Callable>
struct lamda_traits {
    inline static constexpr bool value = true;
};

template <class Callable>
void ff(Callable f) {
    const char* name = abi::__cxa_demangle(typeid(typename xzmjx::function_traits<Callable>::ret_type).name(), nullptr, nullptr, nullptr);
    cout<<name<<endl;
}

int main() {
    //auto lamda = [](){};
    //cout<<is_const<const X>::value<<endl;
    //cout<<is_const<const decltype(lamda)>::value<<endl;
    //cout<<is_const<const void(*)(int)>::value <<endl;
    //using T = PM_traits<decltype(&X::test)>::type1;
    //using U = PM_traits<decltype(&X::test)>::type;
    //cout<< typeid(T).name()<<endl;
    //cout<< typeid(U).name()<<endl;
    //using type = decltype(&X::test);
    //cout<<is_same_v<function_traits<type>::ret_type,int><<endl;
    //cout<<is_same_v<function_traits<type>::class_type ,X><<endl;
    //const char* name = abi::__cxa_demangle(typeid(tuple_element<0,function_traits<type>::tuple_type>::type).name(), nullptr, nullptr, nullptr);
    //cout<< name<<endl;
    //cout<<is_same_v<tuple_element<0,function_traits<type>::tuple_type>::type ,int><<endl;
    //cout<<function_traits<decltype(&X::test)>::value<<endl;
    //cout<<is_const<PM_traits<decltype(&X::test)>::type>::value<<endl;

    auto f = [](){};
    ff(f);
    ff(func);
    //const char* name = abi::__cxa_demangle(typeid(decltype(f)).name(), nullptr, nullptr, nullptr);
    //cout<<name<<endl;
}