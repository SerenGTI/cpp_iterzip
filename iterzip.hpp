#pragma once

#include <cstddef>
#include <iterator>
#include <tuple>

namespace iterzip {

template<typename... Containers>
struct zip {
public:

    using value_type = std::tuple<typename Containers::value_type&...>;
    using pointer = value_type*;
    using refrence = value_type&;
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;

    class Iterator {
    private:
        using iter_pack_t = std::tuple<typename Containers::iterator...>;
        iter_pack_t iters_;
    public:
        Iterator(iter_pack_t in)
            : iters_(std::move(in))
        {}

        auto operator*() const -> value_type {
            return getValue(std::make_index_sequence<sizeof...(Containers)>());
        }

        auto operator++() -> Iterator {
            advanceAll(std::make_index_sequence<sizeof...(Containers)>());
            return *this;
        }

        auto operator!=(const Iterator& other) const -> bool {
            return nequals(iters_, other.iters_);
        }

    private:

        template<std::size_t... Is>
        auto getValue(std::index_sequence<Is...> /*unused*/) const -> value_type {
            return getValue(std::get<Is>(iters_)...);
        }
        template<typename... Iterators>
        auto getValue(Iterators... its) const -> value_type {
            return std::tie(*its...);
        }

        template<std::size_t... Is>
        void advanceAll(std::index_sequence<Is...> /*unused*/) {
            advanceAll(std::get<Is>(iters_)...);
        }
        template<typename Iterator, typename... Iterators>
        void advanceAll(Iterator& iter, Iterators&... iters) {
            ++iter;
            if constexpr (sizeof...(iters) > 0) {
                advanceAll(iters...);
            }
        }

        static auto nequals(const iter_pack_t& lhs, const iter_pack_t& rhs) -> bool {
            return nequals(lhs, rhs, std::make_index_sequence<sizeof...(Containers)>());
        }
        template<std::size_t... Is>
        static auto nequals(const iter_pack_t& lhs, const iter_pack_t& rhs, std::index_sequence<Is...> /*unused*/) -> bool {
            return not ((std::get<Is>(lhs) == std::get<Is>(rhs)) || ...);
        }
    };

    using iterator = Iterator;

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
    constexpr void advanceAll() {

    }
private:
    iterator begin_;
    iterator end_;
};

}