#pragma once
//
//  Copyright © 2021, Alexander Borisov,  https://github.com/SashaBorisov/CppWinApi
//

#include <compare>
#include <type_traits>
#include <iterator>


namespace Utils
{

template<typename T>
struct CountOf;

template<typename T>
struct DiffOf
{
    using Type = T;

    constexpr DiffOf() noexcept = default;
    constexpr DiffOf(const DiffOf&) noexcept = default;
    constexpr DiffOf& operator = (const DiffOf&) noexcept = default;
    constexpr auto operator <=> (const DiffOf& other) const noexcept = default;

    explicit constexpr operator ptrdiff_t () const noexcept
    {
        return value;
    }

    constexpr DiffOf operator - () const noexcept
    {
        return DiffOf{-value};
    }

    constexpr DiffOf operator - (const DiffOf other) const noexcept
    {
        return DiffOf{value - other.value};
    }

    constexpr DiffOf& operator -= (const DiffOf other) noexcept
    {
        value -= other.value;
        return *this;
    }

    constexpr DiffOf operator + (const DiffOf other) const noexcept
    {
        return DiffOf{value + other.value};
    }

    constexpr DiffOf& operator += (const DiffOf other) noexcept
    {
        value += other.value;
        return *this;
    }

    template<typename N> requires std::is_arithmetic_v<N>
    constexpr DiffOf operator * (const N number) const noexcept
    {
        return DiffOf{value * number};
    }

    template<typename N>
    requires std::is_arithmetic_v<N>
    constexpr DiffOf& operator *= (const N number) noexcept
    {
        value *= number;
        return *this;
    }

    template<typename N> requires std::is_arithmetic_v<N>
    constexpr DiffOf operator / (const N number) const noexcept
    {
        return DiffOf{value / number};
    }

    template<typename N> requires std::is_arithmetic_v<N>
    constexpr DiffOf& operator /= (const N number) noexcept
    {
        value /= number;
        return *this;
    }

    template<typename N> requires std::is_integral_v<N>
    constexpr DiffOf operator % (const N number) const noexcept
    {
        return DiffOf{value % number};
    }

    template<typename N> requires std::is_integral_v<N>
    constexpr DiffOf& operator %= (const N number) noexcept
    {
        value %= number;
        return *this;
    }

private:
    friend struct CountOf<T>;
    
    template<typename T>
    friend constexpr DiffOf<std::byte> offsetOf(const DiffOf<T> offset) noexcept;


    constexpr DiffOf(const ptrdiff_t v) noexcept: value{v} {}
    ptrdiff_t value = 0;
};


template<typename T>
struct CountOf
{
    using Type = T;

    constexpr CountOf() noexcept = default;
    constexpr CountOf(const CountOf&) noexcept = default;
    constexpr CountOf& operator = (const CountOf&) noexcept = default;
    constexpr auto operator <=> (const CountOf& other) const noexcept = default;

    explicit constexpr operator size_t () const noexcept
    {
        return value;
    }

    constexpr DiffOf<T> operator - (const CountOf other) const noexcept
    {
        return DiffOf<T>{static_cast<ptrdiff_t>(value) - static_cast<ptrdiff_t>(other.value)};
    }

    constexpr CountOf operator - (const DiffOf<T> diff) const noexcept
    {
        return CountOf{static_cast<size_t>(static_cast<ptrdiff_t>(value) - diff.value)};
    }

    constexpr CountOf& operator -= (const DiffOf<T> diff) noexcept
    {
        value = static_cast<size_t>(static_cast<ptrdiff_t>(value) - diff.value);
        return *this;
    }

    constexpr CountOf operator + (const DiffOf<T> diff) const noexcept
    {
        return CountOf{value + diff.value};
    }

    constexpr CountOf& operator += (const DiffOf<T> diff) noexcept
    {
        value = static_cast<size_t>(static_cast<ptrdiff_t>(value) + diff.value);
        return *this;
    }

    // constexpr CountOf operator + (const CountOf other) const noexcept
    // {
    //     return CountOf{value + other.value};
    // }

    // constexpr CountOf& operator += (const CountOf other) noexcept
    // {
    //     value += other.value;
    //     return *this;
    // }

    template<typename N>
    requires std::is_arithmetic_v<N>
    constexpr CountOf operator * (const N number) const noexcept
    {
        return DiffOf{value * number};
    }

    template<typename N>
    requires std::is_arithmetic_v<N>
    constexpr CountOf operator / (const N number) const noexcept
    {
        return DiffOf{value / number};
    }

private:
    template<typename T>
    friend constexpr CountOf<std::byte> sizeOf() noexcept;

    template <typename C>
    friend constexpr CountOf<typename C::value_type> countOf(const C& container);

    template <typename T, size_t value>
    friend constexpr CountOf<T> countOf(const T (&)[value]) noexcept;

    template<typename T>
    friend constexpr CountOf<std::byte> sizeOf(const CountOf<T> count) noexcept;

    template <typename I>
    friend constexpr CountOf<typename std::iterator_traits<I>::value_type>
    countOf(const I& begin, const I& end);

    template <typename T>
    friend constexpr CountOf<T> countOf(const CountOf<std::byte> size) noexcept;


    constexpr CountOf(const size_t v) noexcept : value{v}{}
    size_t value = 0;

}; // struct CountOf

using OffsetOfBytes = DiffOf<std::byte>;
using CountOfBytes = CountOf<std::byte>;


template<typename T>
constexpr CountOfBytes sizeOf() noexcept
{
    return CountOfBytes{sizeof(T)};
}

template<typename T>
constexpr CountOfBytes sizeOf(T&) noexcept
{
    return sizeOf<T>();
}

constexpr auto OneByte = Utils::sizeOf<std::byte>() - CountOfBytes{};

template<typename T>
constexpr OffsetOfBytes offsetOf(const DiffOf<T> offset) noexcept
{
    return OffsetOfBytes{static_cast<ptrdiff_t>(sizeof(T)) * offset.value};
}

template<typename T>
constexpr CountOfBytes sizeOf(const CountOf<T> count) noexcept
{
    return CountOfBytes{sizeof(T) * count.value};
}


template <typename C>
constexpr CountOf<typename C::value_type> countOf(const C& container)
{
    return CountOf<typename C::value_type>{container.size()};
}

template <typename T, size_t value>
constexpr CountOf<T> countOf(const T (&)[value]) noexcept
{
    return CountOf<T>{value};
}

template <typename I>
constexpr CountOf<typename std::iterator_traits<I>::value_type> 
    countOf(const I& begin, const I& end)
{
    const auto value = static_cast<size_t>(std::distance(begin, end));
    return CountOf<typename std::iterator_traits<I>::value_type>{value};
}

template <typename T>
constexpr CountOf<T> countOf(const CountOfBytes size) noexcept
{
    return CountOf<T>{size.value / sizeof(T)};
}

template <typename T>
constexpr CountOfBytes tailOf(const CountOf<T> count) noexcept
{
    return CountOfBytes{sizeOf(count).value % sizeof(T)};
}


}

