#pragma once
//
//  Copyright © 2021, Alexander Borisov,  https://github.com/SashaBorisov/CppWinApi
//

namespace Utils
{

template<auto V, typename T = decltype(V)>
struct Flag
{
    using Type = decltype(V);
    // using Tag = T;

    constexpr Flag() noexcept = default;
    constexpr Flag(const Flag&) noexcept = default;
    constexpr Flag& operator = (const Flag&) noexcept = default;

    constexpr bool operator == (const Flag other) const noexcept
    {
        return value == other.value;
    }

    constexpr bool operator != (const Flag other) const noexcept
    {
        return value != other.value;
    }

    constexpr Flag operator ~ () const noexcept
    {
        return Flag{~value};
    }

    constexpr Flag operator ! () const noexcept
    {
        return Flag{~value};
    }

private:
    constexpr Flag(const Type v): value{v} {}
    Type value = V;
};

template<bool V, typename T>
struct Flag<V, T>
{
    using Type = bool;
    // using Tag = T;

    constexpr Flag() noexcept = default;
    constexpr Flag(const Flag&) noexcept = default;
    constexpr Flag& operator = (const Flag&) noexcept = default;

    constexpr bool operator == (const Flag other) const noexcept
    {
        return value == other.value;
    }

    constexpr bool operator != (const Flag other) const noexcept
    {
        return value != other.value;
    }

    constexpr Flag operator ~ () const noexcept
    {
        return Flag{!value};
    }

    constexpr Flag operator ! () const noexcept
    {
        return Flag{!value};
    }

private:
    constexpr Flag(const Type v): value{v} {}
    Type value = V;
};

}