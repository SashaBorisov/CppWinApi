#pragma once
//
//  Copyright © 2021, Alexander Borisov,  https://github.com/SashaBorisov/CppWinApi
//

namespace Utils
{

#define UNIQUE_TAG decltype([](){})

template<auto V, typename Tag>
struct Flag
{
    using Type = decltype(V);

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
        static_assert(!std::is_same_v<bool, Type>);
        return Flag{~value};
    }

    constexpr Flag operator ! () const noexcept
    {
        static_assert(std::is_same_v<bool, Type>);
        return Flag{!value};
    }

private:
    template<typename E>
    friend class Mask;

    constexpr Flag(const Type v): value{v} {}
    Type value = V;
};


}
