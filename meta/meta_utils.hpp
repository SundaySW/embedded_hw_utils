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
    }// namespace type_take

    template<typename T, typename ... Ts>
    constexpr size_t index_in_tuple(){
        using TypesTup = std::tuple<Ts...>;
        constexpr auto find_idx = []<std::size_t I>(auto& self){
            static_assert(I < std::tuple_size<TypesTup>::value, "The element is not in the tuple");
            using IElemT = typename std::tuple_element<I, TypesTup>::type;
            if constexpr (std::is_same<T, IElemT>::value)
                return I;
            else
                return self.template operator()<I+1>(self);
        };
        return find_idx.template operator()<0>(find_idx);
    }

    template <typename TIgnore, class... Args, typename F>
    constexpr decltype(auto) remove_arg_by_type_and_invoke(F&& f, Args&&... args){
        constexpr auto Idx = index_in_tuple<TIgnore, Args...>();

        [&]<class... Take>(type_take::list<Take...>) {
            [&](Take&&... take, auto&&, auto&&... rest){
                return std::invoke(std::forward<F>(f), std::forward<Take>(take)...,
                                   std::forward<decltype(rest)>(rest)...);
            }(std::forward<Args>(args)...);
        }(typename type_take::take<Idx, type_take::list<Args...>>::type{});
    }

    template <typename T, typename... Args>
    decltype(auto) magic_get(Args&&... as){
        constexpr auto pos = index_in_tuple<T, Args...>();
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
        return std::make_tuple(std::get<I>(std::forward<Tuple>(tuple)) ...);
    }

    template<typename T, typename... Args>
    decltype(auto) pass_through_without(Args&&... args){
        constexpr auto N = index_in_tuple<T, Args...>();
        []<typename Tuple_t, std::size_t ...I, std::size_t ...Y>
                (Tuple_t&& tup, std::index_sequence<I...>, std::index_sequence<Y...>)
        {
            return std::forward_as_tuple(std::get<I>(std::forward<Tuple_t>(tup))...,
                                         std::get<Y>(std::forward<Tuple_t>(tup))...);
        }(std::forward_as_tuple(std::forward<Args>(args)...),
          std::make_index_sequence<N>{}, make_index_range<N+1, sizeof...(Args)>{});
    }

    template<typename T, typename ... Args>
    concept has_type = (std::is_same_v<T, Args> || ...);

}// namespace meta::utils