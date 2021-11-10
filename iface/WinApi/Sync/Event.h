#pragma once
//
//  Copyright © 2021, Alexander Borisov,  https://github.com/SashaBorisov/CppWinApi
//

#include <WinApi/Common.h>
#include <WinApi/Handle.h>
#include <synchapi.h>


namespace WinApi::Sync
{

enum class EventReset: BOOL
{
      Manual = TRUE
    , Auto   = FALSE
};
constexpr EventReset operator ! (const EventReset mode) noexcept
{
    return mode == EventReset::Manual ? EventReset::Auto : EventReset::Manual;
}

enum class EventState: BOOL
{
         Signaled = TRUE
    , NotSignaled = FALSE
};
constexpr EventState operator ! (const EventState state) noexcept
{
    return state == EventState::Signaled ? EventState::NotSignaled : EventState::Signaled;
}

template<EventReset Mode>
auto createEvent(  const EventState initial = EventState::NotSignaled
                 , const StringView& name = {}  )
{
    const HANDLE handle = ::CreateEvent
    (
          nullptr
        , Mode == EventReset::Manual
        , initial == EventState::Signaled
        , name.data()
    );
    auto event = safeHandle(handle, [](const HANDLE handle)
    {
        if(handle)
        {
            ::CloseHandle(handle);
        }
    });
    using Result = decltype(event);

    if(!event)
    {
        return Maybe<Result>{OccurredError{}};
    }
    return Maybe<Result>{std::move(event)};
}

template<EventReset mode>
using MaybeEvent = decltype(createEvent<mode>(EventState::Signaled, StringView{}));

template<EventReset mode>
using Event = typename MaybeEvent<mode>::Type;

using ManualEvent = Event<EventReset::Manual>;
using AutoEvent   = Event<EventReset::Auto>;

template<typename E>
concept IsItEvent =    std::is_same_v<E, ManualEvent> 
                    || std::is_same_v<E, AutoEvent>;

static Maybe<ManualEvent> createManualEvent(  const EventState initial = !EventState::Signaled
                                            , const StringView& name = {})
{
    return createEvent<EventReset::Manual>(initial, name);
}

static Maybe<AutoEvent> createAutoEvent(  const EventState initial = !EventState::Signaled
                                        , const StringView& name = {})
{
    return createEvent<EventReset::Auto>(initial, name);
}


template<typename E> requires IsItEvent<E>
Maybe<void> setEvent(const E& event) 
{
    if(FALSE != ::SetEvent(event.get()))
    {
        return {};
    }
    return OccurredError{};
}

template<typename E> requires IsItEvent<E>
Maybe<void> resetEvent(const E& event) 
{
    if(FALSE != ::ResetEvent(event.get()))
    {
        return {};
    }
    return OccurredError{};
}


}  // namespace WinApi
