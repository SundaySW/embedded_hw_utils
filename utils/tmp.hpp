#pragma once

#include <iostream>
#include <tuple>
#include <type_traits>
#include <string_view>
#include <functional>

void do_something(bool a, int b, std::string_view c)
{
    std::cout << c << " " << b << " " << std::boolalpha << a << std::endl;
}

template<typename R, typename... Args>
struct FunctionSignature;

template<typename R, typename... Args>
struct FunctionSignature<R(*)(Args...)> {
    using args_t = std::tuple<Args...>;
    using result_t = R;
};

template <class T, class Tuple>
struct TupleIndex;

template <class T, class... Types>
struct TupleIndex<T, std::tuple<T, Types...>> {
    static inline constexpr std::size_t value = 0;
};

template <class T, class U, class... Types>
struct TupleIndex<T, std::tuple<U, Types...>> {
    static inline constexpr std::size_t value = 1 + TupleIndex<T, std::tuple<Types...>>::value;
};

template<typename SourceTuple, typename TargetTuple, std::size_t... I>
constexpr auto BindIndexes(std::index_sequence<I...>) {
    return std::integer_sequence<std::size_t, TupleIndex<std::tuple_element_t<I, TargetTuple>, SourceTuple>::value...>{};
}

template<typename F, typename Tuple, std::size_t... I>
constexpr decltype(auto) Apply(F&& f, Tuple&& tuple, std::index_sequence<I...>) {
    return std::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(tuple))...);
}

template<typename F, typename... T>
constexpr decltype(auto) InvokeRandomArgs(F&& f, T&&... args) {
    using SourceArgs = std::tuple<T...>;
    using SourceSequence = std::index_sequence_for<T...>;
    using TargetArgs = typename FunctionSignature<F>::args_t;
    return Apply(std::forward<F>(f), std::forward_as_tuple(std::forward<T>(args)...), BindIndexes<SourceArgs, TargetArgs>(SourceSequence{}));
}

//    InvokeRandomArgs(&do_something, std::string_view{ "Hello" }, true, 20);
//    InvokeRandomArgs(&do_something, false, 40, std::string_view{ "World" });
//    InvokeRandomArgs(&do_something, 14, std::string_view{ "C++" }, true);
//    InvokeRandomArgs(&do_something, 17, true, std::string_view{ "std" });
