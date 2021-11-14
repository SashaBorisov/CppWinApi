#pragma once
//
//  Copyright © 2021, Alexander Borisov,  https://github.com/SashaBorisov/CppWinApi
//

#include <WinApi/Common.h>
#include <winnt.h>
#include <memory>
#include <Utils/Flag.h>


namespace WinApi
{

template<typename C>
using Handle = std::unique_ptr<void, C>;

template<typename C>
Handle<C> safeHandle(const HANDLE asset, C&& close) noexcept
{
    return Handle<C>(asset, std::move(close));
}

using AlertableFlag = Utils::Flag<true, UNIQUE_TAG>;
constexpr auto Alertable = AlertableFlag{};

enum class WaitStatus: DWORD
{
      Abandoned     = WAIT_ABANDONED
    , IoCompletion  = WAIT_IO_COMPLETION
    , Object0       = WAIT_OBJECT_0
    , Timeout       = WAIT_TIMEOUT
};

template<typename C>
Maybe<WaitStatus> waitFor(    const Handle<C>& handle
                            , const Milliseconds timeout = Infinite
                            , const AlertableFlag alertable = !Alertable  )
{
    const DWORD status = ::WaitForSingleObjectEx
    (
          handle.get()
        , timeout.count()
        , alertable == Alertable ? TRUE : FALSE
    );
    if(status == WAIT_FAILED)
    {
        return OccurredError{};
    }
    return static_cast<WaitStatus>(status);
}

}