#pragma once
//
//  Copyright © 2021, Alexander Borisov,  https://github.com/SashaBorisov/CppWinApi
//

#include <WinApi/Common.h>
#include <WinApi/Handle.h>
#include <heapapi.h>
#include <Utils/CountOf.h>



namespace WinApi
{

using Utils::DiffOf;
using Utils::CountOf;
using Utils::OffsetOfBytes;
using Utils::CountOfBytes;
using Utils::OneByte;


struct HeapFlags
{
    static constexpr DWORD EnableExecute        = HEAP_CREATE_ENABLE_EXECUTE;
    static constexpr DWORD GenerateExceptions   = HEAP_GENERATE_EXCEPTIONS;
    static constexpr DWORD NoSerialize          = HEAP_NO_SERIALIZE;

    template<DWORD Flags>
    static constexpr bool valid() noexcept
    {
        return Flags == (Flags & (EnableExecute | GenerateExceptions | NoSerialize));
    }
};

template<DWORD Flags>
concept IsValidHeapFlags = Flags == (Flags & (    HeapFlags::EnableExecute 
                                                | HeapFlags::GenerateExceptions 
                                                | HeapFlags::NoSerialize));

template<DWORD F, typename U, typename C>
struct Heap: public Handle<C>
{
    using Base = typename Handle<C>;
    static constexpr DWORD Flags = F;
    using Type = U;

    using Base::Base;
};

template<DWORD Flags, typename T = std::byte>
requires IsValidHeapFlags<Flags>
auto createHeap(  const CountOf<T> initialCount = CountOf<T>{}
                , const CountOf<T> maxCount = CountOf<T>{})
{
    const CountOfBytes initialSize = Utils::sizeOf(initialCount);
    const CountOfBytes maxSize     = Utils::sizeOf(maxCount);
    const HANDLE handle = ::HeapCreate
    (
          Flags
        , static_cast<size_t>(initialSize)
        , static_cast<size_t>(maxSize)
    );

    auto destroy = [](const HANDLE handle)
    {
        if(handle)
        {
            ::HeapDestroy(handle);
        }
    };

    using ResultHeap = Heap<Flags, T, decltype(destroy)>;
    if(!handle)
    {
        return Maybe<ResultHeap>{OccurredError{}};
    }
    return Maybe<ResultHeap>{ResultHeap{handle, std::move(destroy)}};
}

template<DWORD Flags, typename T>
using MaybeHeap = decltype(createHeap<Flags, T>());
template<DWORD Flags, typename T>
using HeapTag = typename MaybeHeap<Flags, T>::Type::deleter_type;

template<DWORD Flags, typename T, typename ...A>
requires IsValidHeapFlags<Flags>
auto heapEmplace(const Heap<Flags, T, HeapTag<Flags, T>>& heap, A&&... args)
{
    const auto size = Utils::sizeOf<T>();
    auto destructor = [heap = heap.get()](T* const instance)
    {
        instance->~T();
        ::HeapFree(heap, 0, instance);
    };
    using Instance = std::unique_ptr<T, decltype(destructor)>;
    const auto pointer = ::HeapAlloc(heap.get(), 0, static_cast<size_t>(size));

    if constexpr(Flags & HeapFlags::GenerateExceptions)
    {
        auto holder = makeHandle(pointer, [heap = heap.get()](void* const pointer)
        {
            ::HeapFree(heap, pointer);
        });
        T* const instance = new(pointer) T{std::move(args)...};
        holder.detach();
        return Instance{instance, std::move(destructor)};
    }
    else
    {
        if(!pointer)
        {
            return Maybe<Instance>{OccurredError{}};
        }
        auto holder = safeHandle(pointer, [heap = heap.get()](void* const pointer)
        {
            ::HeapFree(heap, 0, pointer);
        });
        T* const instance = new(pointer) T{std::move(args)...};
        holder.release();
        return Maybe<Instance>{Instance{instance, std::move(destructor)}};
    }
}

} // namespace WinApi
