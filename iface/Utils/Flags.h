#pragma once
//
//  Copyright © 2021, Alexander Borisov,  https://github.com/SashaBorisov/CppWinApi
//

#include <type_traits>


namespace Utils
{

template<typename E>
class Flags
{
    static_assert((std::is_enum_v<E>), "");

public:
    using Enum = E;
    using Type = std::underlying_type_t<Enum>;

    constexpr Flags(const Enum flag) noexcept 
        : value{cast(flag)} 
    {}
    constexpr Flags() noexcept = delete;
    constexpr Flags(const Flags& other) noexcept = default;
    //     : value{other.value}
    // {}
    constexpr Flags& operator = (const Flags& other) noexcept = default;

    constexpr bool operator == (const Flags other) const noexcept
    {
        return value == other.value;
    }

    constexpr bool operator != (const Flags other) const noexcept
    {
        return value != other.value;
    }

private:
    constexpr Flags(Type v) noexcept
        : value{v} 
    {}

    static constexpr Type cast(const Enum flag) noexcept
    {
        return static_cast<Type>(flag);
    }

    Type value;

public:
    constexpr inline Flags& operator &= (Flags other) noexcept { value &= other.value; return *this; }
    constexpr inline Flags& operator |= (Flags other) noexcept { value |= other.value; return *this; }
    constexpr inline Flags& operator ^= (Flags other) noexcept { value ^= other.value; return *this; }
    // constexpr inline Flags& operator &= (Enum  other) noexcept { value &= cast(other); return *this; }
    // constexpr inline Flags& operator |= (Enum  other) noexcept { value |= cast(other); return *this; }
    // constexpr inline Flags& operator ^= (Enum  other) noexcept { value ^= cast(other); return *this; }

    constexpr explicit operator Type() const noexcept { return value; }

    constexpr inline Flags operator | (Flags other) const noexcept { return {value | other.value}; }
    constexpr inline Flags operator ^ (Flags other) const noexcept { return {value ^ other.value}; }
    constexpr inline Flags operator & (Flags other) const noexcept { return {value & other.value}; }
    // constexpr inline Flags operator ^ (Enum  other) const noexcept { return {value ^ cast(other)}; }
    // constexpr inline Flags operator | (Enum  other) const noexcept { return {value | cast(other)}; }
    // constexpr inline Flags operator & (Enum  other) const noexcept { return {value & cast(other)}; }
    constexpr inline Flags operator ~ () const noexcept { return {~value}; }

    constexpr inline void operator + (Flags other) const noexcept = delete;
    constexpr inline void operator + (Enum  other) const noexcept = delete;
    constexpr inline void operator - (Flags other) const noexcept = delete;
    constexpr inline void operator - (Enum  other) const noexcept = delete;
};

} // namespace Utils

// template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
// constexpr Utils::Flags<E> operator ~ (const E flag) noexcept
// {
//     return ~Utils::Flags<E>{left};
// }

// template<typename E>
// constexpr Utils::Flags<E> operator & (const E left, const Utils::Flags<E> right) noexcept
// {
//     return Utils::Flags<E>{left} & right;
// }

// template<typename E>
// constexpr Utils::Flags<E> operator ^ (const E left, const Utils::Flags<E> right) noexcept
// {
//     return Utils::Flags<E>{left} ^ right;
// }

// template<typename E>
// constexpr Utils::Flags<E> operator | (const E left, const Utils::Flags<E> right) noexcept
// {
//     return Utils::Flags<E>{left} | right;
// }

// template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
// constexpr Utils::Flags<E> operator & (const E left, const E right) noexcept
// {
//     return Utils::Flags<E>{left} & Utils::Flags<E>{right};
// }

// template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
// constexpr Utils::Flags<E> operator ^ (const E left, const E right) noexcept
// {
//     return Utils::Flags<E>{left} ^ Utils::Flags<E>{right};
// }

// template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
// constexpr Utils::Flags<E> operator | (const E left, const E right) noexcept
// {
//     return Utils::Flags<E>{left} | Utils::Flags<E>{right};
// }