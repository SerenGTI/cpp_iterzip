#pragma once
#ifndef CPP_ITERZIP_HPP
#define CPP_ITERZIP_HPP

#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

namespace iterzip {

namespace detail {
    template<typename Iterator>
    struct iter_map : std::integral_constant<int, 0> {};
    template<>
    struct iter_map<std::input_iterator_tag> : std::integral_constant<int, 1> {};
    template<>
    struct iter_map<std::output_iterator_tag> : std::integral_constant<int, 1> {};
    template<>
    struct iter_map<std::forward_iterator_tag> : std::integral_constant<int, 2> {};
    template<>
    struct iter_map<std::bidirectional_iterator_tag> : std::integral_constant<int, 3> {};
    template<>
    struct iter_map<std::random_access_iterator_tag> : std::integral_constant<int, 4> {};

    template<typename V1, typename V2>
    struct min : std::integral_constant<int, std::min(V1::value, V2::value)> {};

    template<int Iterator>
    struct map_iter { using tag = std::input_iterator_tag; }; // fallback case is the input iterator
    template<>
    struct map_iter<2> { using tag = std::forward_iterator_tag; };
    template<>
    struct map_iter<3> { using tag = std::bidirectional_iterator_tag; };
    template<>
    struct map_iter<4> { using tag = std::random_access_iterator_tag; };


    template<typename... T>
    struct cat_trait {};
    template<typename T>
    struct cat_trait<T>
    {
        using tag = typename std::iterator_traits<T>::iterator_category;
    };
    template<typename T1, typename... T>
    struct cat_trait<T1, T...>
    {
        using tag = typename map_iter<min<
                iter_map<typename cat_trait<T1>::tag>, 
                iter_map<typename cat_trait<T...>::tag>
            >::value>::tag;
    };
    template<typename... Ts>
    using cat_trait_t = typename cat_trait<Ts...>::tag;
} // namespace detail


namespace impl {
    template<typename... Ts, typename Map, typename Acc, typename A, std::size_t... I>
    static inline auto foldPairwise(const std::tuple<Ts...>& lhs, const std::tuple<Ts...>& rhs, Map&& map, Acc&& acc, A init, std::index_sequence<I...>)
    {
        return acc(init, map(std::get<I>(lhs), std::get<I>(rhs)) ...);
    }
    template<typename... Ts, typename Map, typename Acc, typename A>
    static inline auto foldPairwise(const std::tuple<Ts...>& lhs, const std::tuple<Ts...>& rhs, Map&& map, Acc&& acc, A init) -> decltype(auto)
    {
        return foldPairwise(lhs, rhs, std::forward<Map>(map), std::forward<Acc>(acc), init, std::make_index_sequence<sizeof...(Ts)>());
    }
} // namespace impl

// iterzip::iterator_traits extends std::iterator_traits to packs of iterator types,
// uses std::tuple<...> to represent a list of types.
// Fully compatible with std::iterator_traits for a single given iterator type
template<typename... Iters>
struct iterator_traits;
template<typename Iterator>
struct iterator_traits<Iterator> : std::iterator_traits<Iterator> {};
template<typename... Iterators>
struct iterator_traits
{
    using value_type        = std::tuple<typename iterator_traits<Iterators>::value_type ...>;
    using reference         = std::tuple<typename iterator_traits<Iterators>::reference ...>;
    using pointer           = reference*;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = detail::cat_trait_t<Iterators...>;
};



namespace detail {
    template<typename Iterator, typename Category>
    struct is_at_least
    {
        constexpr static bool value = detail::iter_map<typename iterator_traits<Iterator>::iterator_category>::value >= detail::iter_map<Category>::value;
    };
}


template<typename... Containers>
struct zip {
public:

    class Iterator {
    private:
        using iter_pack_t = std::tuple<typename Containers::iterator...>;
        iter_pack_t iters_;
    public:

        using value_type        = typename iterator_traits<typename Containers::iterator ...>::value_type;
        using pointer           = typename iterator_traits<typename Containers::iterator ...>::pointer;
        using reference         = typename iterator_traits<typename Containers::iterator ...>::reference;
        using iterator_category = typename iterator_traits<typename Containers::iterator ...>::iterator_category;
        using difference_type   = typename iterator_traits<typename Containers::iterator ...>::difference_type;



        // pre-increment
        auto operator++() -> Iterator
        {
            std::apply([](auto&&... iters){
                (++iters, ...);
            }, iters_);
            return *this;
        }
        // post-increment
        auto operator++(int) -> Iterator
        {
            iter_pack_t pre_state = std::apply([](auto&&... iters){
                return std::tuple{iters++ ...};
            }, iters_);
            return {pre_state};
        }




        //
        //  Input iterator functions
        //  

        auto operator*() const -> value_type
        {
            return std::apply([](auto&&... iters){
                return std::tuple(*iters...);
            }, iters_);
        }

        auto operator!=(const Iterator& other) const -> bool
        {
            // compares individual iterators pair-wise
            auto map = [](auto&& iter1, auto&& iter2) {
                return iter1 == iter2;
            };
            // if any of the iterator pairs are equal, return true
            auto acc = [](auto init, auto&&... vals) {
                return (init || ... || vals);
            };
            return not impl::foldPairwise(iters_, other.iters_, map, acc, false);
        }

        auto operator==(const Iterator& other) const -> bool
        {
            return not *this != other;
        }

        // 
        // Output iterator functions
        // 

        auto operator*() -> reference
        {
            return std::apply([](auto&&... iters){
                return std::tie(*iters...);
            }, iters_);
        }


        // 
        // Bidirectional iterator functions
        // 
        
        Iterator() = default;


        // pre-decrement
        auto operator--() -> Iterator
        {
            std::apply([](auto&&... iters){
                (--iters, ...);
            }, iters_);
            return *this;
        }
        // post-decrement
        auto operator--(int) -> Iterator
        {
            iter_pack_t pre_state = std::apply([](auto&&... iters){
                return std::tuple{iters-- ...};
            }, iters_);
            return {pre_state};
        }


        // 
        // Random-access iterator functions
        // 
        
        auto operator+(difference_type offset) -> Iterator
        {
            return std::apply([offset](auto&&... iters){
                return std::tuple{(iters + offset) ...};
            }, iters_);
        }
        auto operator+(Iterator other) -> Iterator
        {}
        auto operator-(difference_type offset) -> Iterator
        {
            return std::apply([offset](auto&&... iters){
                return std::tuple{(iters - offset) ...};
            }, iters_);
        }
        auto operator-(Iterator other) -> Iterator
        {}


        auto operator<(Iterator other) -> bool
        {
            // compares individual iterators pair-wise
            auto map = [](auto&& iter1, auto&& iter2) {
                return iter1 < iter2;
            };
            // if all iterator pairs are <, return true
            auto acc = [](auto init, auto&&... vals) {
                return (init && ... && vals);
            };
            return not impl::foldPairwise(iters_, other.iters_, map, acc, true);
        }
        auto operator<=(Iterator other) -> bool
        {
            // compares individual iterators pair-wise
            auto map = [](auto&& iter1, auto&& iter2) {
                return iter1 <= iter2;
            };
            // if all iterator pairs are <=, return true
            auto acc = [](auto init, auto&&... vals) {
                return (init && ... && vals);
            };
            return not impl::foldPairwise(iters_, other.iters_, map, acc, true);
        }
        auto operator>(Iterator other) -> bool
        {
            // compares individual iterators pair-wise
            auto map = [](auto&& iter1, auto&& iter2) {
                return iter1 > iter2;
            };
            // if all iterator pairs are >, return true
            auto acc = [](auto init, auto&&... vals) {
                return (init && ... && vals);
            };
            return not impl::foldPairwise(iters_, other.iters_, map, acc, true);
        }
        auto operator>=(Iterator other) -> bool
        {
            // compares individual iterators pair-wise
            auto map = [](auto&& iter1, auto&& iter2) {
                return iter1 >= iter2;
            };
            // if all iterator pairs are >=, return true
            auto acc = [](auto init, auto&&... vals) {
                return (init && ... && vals);
            };
            return not impl::foldPairwise(iters_, other.iters_, map, acc, true);
        }


        auto operator+=(difference_type offset) -> Iterator
        {
            return *this + offset;
        }
        auto operator-=(difference_type offset) -> Iterator
        {
            return *this - offset;
        }


        auto operator[](std::size_t index) const -> value_type
        {
            return std::apply([index](auto&&... iters){
                return std::tuple{iters[index] ...};
            }, iters_);
        }
        auto operator[](std::size_t index) -> reference
        {
            return std::apply([index](auto&&... iters){
                return std::tie(iters[index] ...);
            }, iters_);
        }

    private:
        friend zip;

        Iterator(iter_pack_t in)
            : iters_(std::move(in))
        {}

    };
    using iterator = Iterator;

    // all categories must fulfill
    static_assert(std::is_copy_constructible_v<iterator>);
    static_assert(std::is_copy_assignable_v<iterator>);
    static_assert(std::is_destructible_v<iterator>);

    // forward iterator and up must be default constructible
    static_assert(not detail::is_at_least<iterator, std::forward_iterator_tag>::value || std::is_default_constructible_v<iterator>);


    zip(Containers&... conts) 
        : begin_(std::make_tuple(conts.begin()...)),
          end_(std::make_tuple(conts.end()...))
    {}

    constexpr auto begin() const noexcept -> iterator {
        return begin_;
    }

    constexpr auto end() const noexcept -> iterator {
        return end_;
    }

private:
    iterator begin_;
    iterator end_;
};

} // namespace iterzip

#endif