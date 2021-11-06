#pragma once
//
//  Copyright © 2021, Alexander Borisov,  https://github.com/SashaBorisov/CppWinApi
//

#include <windows.h>
#include <tchar.h>
#include <string>
#include <string_view>
#include <chrono>
#include <format>
#include <system_error>


namespace WinApi
{
using String     = std::basic_string<TCHAR>;
using StringView = std::basic_string_view<TCHAR>;

using Milliseconds = std::chrono::duration<DWORD, std::milli>;
static constexpr auto Infinite = Milliseconds{INFINITE};


static std::error_code lastErrorCode()
{
    const DWORD code = ::GetLastError();
    return std::error_code{static_cast<int>(code), std::system_category()};
}

static std::string lastErrorMessage()
{
    const DWORD code = ::GetLastError();
    return std::system_category().message(static_cast<int>(code));
}

struct OccurredError
{
    OccurredError() noexcept = default;
    constexpr OccurredError(const OccurredError&) noexcept = default;
    constexpr OccurredError& operator = (const OccurredError&) noexcept = default;

    constexpr bool operator == (const OccurredError other) const noexcept
    {
        return value == other.value;
    }

    std::error_code code() const noexcept
    {
        return std::error_code{static_cast<int>(value), std::system_category()};
    }

    std::string message() const
    {
        return std::system_category().message(static_cast<int>(value));
    }

private:
    template<typename T>
    friend struct Maybe;

    struct Empty{};
    constexpr OccurredError(Empty) noexcept : value{0u} {}

    DWORD value = ::GetLastError();

}; // struct OccurredError

template<typename T>
struct Maybe
{
    using Type = T;

    constexpr Maybe() = delete;
    constexpr Maybe(const Maybe&) = delete;
    constexpr Maybe(Maybe&&) noexcept(std::is_nothrow_move_constructible_v<T>) = default;

    constexpr Maybe(const OccurredError lastError) noexcept(std::is_nothrow_default_constructible<T>)
        : error{lastError} 
    {}

    constexpr Maybe(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>) 
        : asset{std::move(value)}
    {}

    constexpr OccurredError code() const noexcept
    {
        return error;
    }

    std::string message() const
    {
        return error.message();
    }

    constexpr bool okay() const noexcept 
    {
        return  NoError == error;
    }

    template<typename ...A>
    const T& unWrap(A&&... args) const & 
    {
        throwIfWrong(std::move(args)...);
        return asset;
    }

    template<typename ...A>
    T&& unWrap(A&&... args) &&
    {
        throwIfWrong(std::move(args)...);
        return std::move(asset);
    }

    const T& value() const & 
    {
        throwIfWrong();
        return asset;
    }

    T& value() & 
    {
        throwIfWrong();
        return asset;
    }

    T&& value() && 
    {
        throwIfWrong();
        return std::move(asset);
    }

private:

    template<typename ...A>
    void throwIfWrong(A&&... args) const
    {
        if(!okay())
        {
            std::string message = std::format(std::move(args)...);
            throw std::system_error{error.code(), std::move(message)};
        }
    }

    void throwIfWrong() const
    {
        if(!okay())
        {
            throw std::system_error{error.code()};
        }
    }

    static constexpr auto NoError = OccurredError{OccurredError::Empty{}};
    OccurredError error = NoError;
    T asset;

}; // struct Maybe

template<>
struct Maybe<void>
{
    using Type = void;
    
    constexpr Maybe() noexcept = default;
    constexpr Maybe(Maybe&&) noexcept = default;
    
    constexpr Maybe(const OccurredError lastError) noexcept
        : error{lastError} 
    {}

    template<typename U>
    constexpr Maybe(Maybe<U>&& copy) noexcept 
        : Maybe{copy.code()} 
    {}

    constexpr OccurredError code() const noexcept
    {
        return error;
    }

    std::string message() const
    {
        return error.message();
    }

    constexpr bool okay() const noexcept 
    {
        return  NoError == error;
    }

    template<typename ...A>
    void unWrap(A&&... args) const
    {
        throwIfWrong(std::move(args)...);
    }

    void value() const
    {
        throwIfWrong();
    }

private:

    template<typename ...A>
    void throwIfWrong(A&&... args) const
    {
        if(!okay())
        {
            std::string message = std::format(std::move(args)...);
            throw std::system_error{error.code(), std::move(message)};
        }
    }

    void throwIfWrong() const
    {
        if(!okay())
        {
            throw std::system_error{error.code()};
        }
    }

    static constexpr auto NoError = OccurredError{OccurredError::Empty{}};
    OccurredError error = NoError;

}; // struct Maybe<void>

} // namespace WinApi

