#pragma once

#include <cstddef>
#include <functional>
#include <tuple>

namespace meta::utils{

    namespace type_take{
        template <class...>
        struct list {};

        template <std::size_t, class, class = list<>>
        struct take;

        template <class Drop, class Take>
        struct take<0, Drop, Take> {
            using type = Take;
        };

        template <std::size_t N, class T, class... Drop, class... Take>
        requires(N > 0)
        struct take<N, list<T, Drop...>, list<Take...> >
                :take<N - 1, list<Drop...>, list<Take..., T> >
        {};
    }  // namespace detail

    template<size_t I, typename T, typename Tuple_t>
    constexpr size_t index_in_tuple_helper(){
        static_assert(I < std::tuple_size<Tuple_t>::value, "The element is not in the tuple");
        using el = typename std::tuple_element<I,Tuple_t>::type;
        if constexpr(std::is_same<T,el>::value )
            return I;
        else
            return index_in_tuple_helper<I + 1, T, Tuple_t>();
    }

    template<typename T, typename ...Args>
    struct index_in_tuple{
        static constexpr size_t value = index_in_tuple_helper<0, T, std::tuple<Args...>>();
    };

    template <typename TIgnore, class... Args, typename F>
    constexpr decltype(auto) remove_arg_by_type_and_invoke(F&& f, Args&&... arguments){
        constexpr auto Idx = index_in_tuple<TIgnore, Args...>::value;
        [&]<class... Take>(type_take::list<Take...>) {
            [&](Take&&... take, auto&&, auto&&... rest){
                return std::invoke(std::forward<F>(f), std::forward<Take>(take)...,
                                   std::forward<decltype(rest)>(rest)...);
            }(std::forward<Args>(arguments)...);
        }(typename type_take::take<Idx, type_take::list<Args...>>::type{});
    }

    template <typename T, typename... Args>
    decltype(auto) magic_get(Args&&... as){
        constexpr auto pos = index_in_tuple<T, Args...>::value;
        return std::get<pos>(std::forward_as_tuple(std::forward<Args>(as)...));
    }

    template<std::size_t N, std::size_t... Seq>
    constexpr decltype(auto) add(std::index_sequence<Seq...>){
        return std::index_sequence<N + Seq ...>{};
    }

    template<std::size_t Min, std::size_t Max>
    using make_index_range = decltype(add<Min>(std::make_index_sequence<Max-Min>()));

    template<std::size_t ... I, typename Tuple>
    decltype(auto) get_tuple_range(Tuple&& tuple, std::index_sequence<I...>){
        return std::make_tuple(std::get<I>(std::forward<Tuple>(tuple))...);
    }

    template<typename T, typename... Args>
    decltype(auto) pass_through_without(Args&&... args){
        constexpr auto N = index_in_tuple<T, Args...>::value;
        auto tup = std::make_tuple(std::forward<Args>(args)...);
        []<typename Tuple_t, std::size_t ...I, std::size_t ...Y>
                (Tuple_t&& tup, std::index_sequence<I...>, std::index_sequence<Y...>)
        {
            return std::tuple_cat(std::get<I>(std::forward<decltype(tup)>(decltype(tup)(tup)))...,
                                  std::get<Y>(std::forward<decltype(tup)>(decltype(tup)(tup)))...);
        }(std::forward_as_tuple(std::forward<Args>(args)...), std::make_index_sequence<N>{}, make_index_range<N+1, sizeof...(Args)>{});
    }

    template<typename What, typename ... Args>
    concept has_type = (std::is_same_v<What, Args> || ...);

}// namespace meta::utils