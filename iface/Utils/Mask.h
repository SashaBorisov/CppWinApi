#pragma once
//
//  Copyright © 2021, Alexander Borisov,  https://github.com/SashaBorisov/CppWinApi
//

#include <type_traits>
#include <Utils/Flag.h>


namespace Utils
{

template<typename E>
class Mask
{
public:
    using Enum = E;
    using Type = typename Enum::Type;

    template<auto V>
    constexpr Mask(const Flag<V,Enum> flag) noexcept 
        : value{cast(flag)} 
    {}
    constexpr Mask() noexcept = delete;
    constexpr Mask(const Mask& other) noexcept = default;
    constexpr Mask& operator = (const Mask& other) noexcept = default;

    constexpr bool operator == (const Mask other) const noexcept
    {
        return value == other.value;
    }

    constexpr bool operator != (const Mask other) const noexcept
    {
        return value != other.value;
    }

    explicit constexpr operator Type() const noexcept
    {
        return value;
    }

private:
    constexpr Mask(Type v) noexcept
        : value{v} 
    {}

    template<auto V>
    static constexpr Type cast(const Flag<V,Enum> flag) noexcept
    {
        return static_cast<Type>(flag.value);
    }

    Type value;

public:
    constexpr inline Mask& operator &= (Mask other) noexcept { value &= other.value; return *this; }
    constexpr inline Mask& operator |= (Mask other) noexcept { value |= other.value; return *this; }
    constexpr inline Mask& operator ^= (Mask other) noexcept { value ^= other.value; return *this; }

    template<auto V> constexpr inline Mask& operator &= (Flag<V,Enum> other) noexcept { value &= cast(other); return *this; }
    template<auto V> constexpr inline Mask& operator |= (Flag<V,Enum> other) noexcept { value |= cast(other); return *this; }
    template<auto V> constexpr inline Mask& operator ^= (Flag<V,Enum> other) noexcept { value ^= cast(other); return *this; }

    constexpr inline Mask operator | (Mask other) const noexcept { return {value | other.value}; }
    constexpr inline Mask operator ^ (Mask other) const noexcept { return {value ^ other.value}; }
    constexpr inline Mask operator & (Mask other) const noexcept { return {value & other.value}; }
    
    template<auto V> constexpr inline Mask operator ^ (Flag<V,Enum> other) const noexcept { return {value ^ cast(other)}; }
    template<auto V> constexpr inline Mask operator | (Flag<V,Enum> other) const noexcept { return {value | cast(other)}; }
    template<auto V> constexpr inline Mask operator & (Flag<V,Enum> other) const noexcept { return {value & cast(other)}; }

    constexpr inline Mask operator ~ () const noexcept { return {~value}; }

    constexpr inline void operator + (Mask other) const noexcept = delete;
    constexpr inline void operator + (Enum other) const noexcept = delete;
    constexpr inline void operator - (Mask other) const noexcept = delete;
    constexpr inline void operator - (Enum other) const noexcept = delete;
};

template<typename E, auto L>
constexpr Mask<E> operator & (const Flag<L,E> left, const Mask<E> right) noexcept
{
    return Utils::Mask<E>{left} & right;
}

template<typename E, auto L>
constexpr Mask<E> operator ^ (const Flag<L,E> left, const Mask<E> right) noexcept
{
    return Utils::Mask<E>{left} ^ right;
}

template<typename E, auto L>
constexpr Mask<E> operator | (const Flag<L,E> left, const Mask<E> right) noexcept
{
    return Mask<E>{left} | right;
}

template<typename E, auto L, auto R>
constexpr Mask<E> operator & (const Flag<L,E> left, const Flag<R,E> right) noexcept
{
    return Mask<E>{left} & Mask<E>{right};
}

template<typename E, auto L, auto R>
constexpr Mask<E> operator ^ (const Flag<L,E> left, const Flag<R,E> right) noexcept
{
    return Utils::Mask<E>{left} ^ Mask<E>{right};
}

template<typename E, auto L, auto R>
constexpr Mask<E> operator | (const Flag<L,E> left, const Flag<R,E> right) noexcept
{
    return Mask<E>{left} | Mask<E>{right};
}


} // namespace Utils
