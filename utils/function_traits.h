//
// Created by 20132 on 2022/11/30.
//

#ifndef XZMJX_FUNCTION_TRAITS_H
#define XZMJX_FUNCTION_TRAITS_H
#include <functional>
namespace xzmjx {
template <class>
struct function_traits;

template <class Ret, class... Args>
struct function_traits<Ret(Args...)> {
  using ret_type = Ret;
  using arg_tuple_type = std::tuple<std::remove_cvref_t<Args>...>;
  using function_type = Ret(Args...);
  using stl_function_type = std::function<function_type>;
  using function_ptr = Ret (*)(Args...);
};

template <class Ret, class... Args>
struct function_traits<Ret (*)(Args...)> : function_traits<Ret(Args...)> {};

template <class Ret, class... Args>
struct function_traits<std::function<Ret(Args...)>>
    : function_traits<Ret(Args...)> {};

template <class Ret, class ClassType, class... Args>
struct function_traits<Ret (ClassType::*)(Args...)>
    : function_traits<Ret(Args...)> {};

template <class Ret, class ClassType, class... Args>
struct function_traits<Ret (ClassType::*)(Args...) const>
    : function_traits<Ret(Args...)> {};

template <class Ret, class ClassType, class... Args>
struct function_traits<Ret (ClassType::*)(Args...) const volatile>
    : function_traits<Ret(Args...)> {};

template <class Ret, class ClassType, class... Args>
struct function_traits<Ret (ClassType::*)(Args...) volatile>
    : function_traits<Ret(Args...)> {};

template <class Callable>
struct function_traits : function_traits<decltype(&Callable::operator())> {};

}  // namespace xzmjx

#endif  // XZMJX_FUNCTION_TRAITS_H
