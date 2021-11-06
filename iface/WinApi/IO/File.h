#pragma once
//
//  Copyright © 2021, Alexander Borisov,  https://github.com/SashaBorisov/CppWinApi
//

#include <type_traits>
#include <filesystem>
#include <optional>
#include <WinApi/Handle.h>
#include <Utils/Flags.h>
#include <Utils/CountOf.h>


namespace WinApi::IO
{

using Utils::DiffOf;
using Utils::CountOf;
using Utils::OffsetOfBytes;
using Utils::CountOfBytes;
using Utils::OneByte;


// // FILE_GENERIC_READ;
enum class DesiredAccess: DWORD
{
      GenericAll       = GENERIC_ALL
    , GenericExecute   = GENERIC_EXECUTE
    , GenericRead      = GENERIC_READ
    , GenericWrite     = GENERIC_WRITE
    , GenericReadWrite = GENERIC_READ | GENERIC_WRITE
};

enum class ShareFlag: DWORD
{
      None   = 0
    , Delete = FILE_SHARE_DELETE
    , Read   = FILE_SHARE_READ
    , Write  = FILE_SHARE_WRITE
};
using ShareFlags = Utils::Flags<ShareFlag>;

enum class CreateMode: DWORD
{
      CreateAlways      = CREATE_ALWAYS
    , CreateNew         = CREATE_NEW
    , OpenAlways        = OPEN_ALWAYS
    , OpenExisting      = OPEN_EXISTING
    , TruncateExisting  = TRUNCATE_EXISTING
};

enum class FileFlag: DWORD
{
      Archive   = FILE_ATTRIBUTE_ARCHIVE
    , Encripted = FILE_ATTRIBUTE_ENCRYPTED
    , Hidden    = FILE_ATTRIBUTE_HIDDEN
    , Normal    = FILE_ATTRIBUTE_NORMAL
    , Offline   = FILE_ATTRIBUTE_OFFLINE
    , ReadOnly  = FILE_ATTRIBUTE_READONLY
    , System    = FILE_ATTRIBUTE_SYSTEM
    , Temporary = FILE_ATTRIBUTE_TEMPORARY

    , BackupSematics    = FILE_FLAG_BACKUP_SEMANTICS
    , DeleteOnClose     = FILE_FLAG_DELETE_ON_CLOSE
    , NoBuffering       = FILE_FLAG_NO_BUFFERING
    , OpenNoRecall      = FILE_FLAG_OPEN_NO_RECALL
    , OpenReparsePoint  = FILE_FLAG_OPEN_REPARSE_POINT
    , Overllaped        = FILE_FLAG_OVERLAPPED
    , PosixSematics     = FILE_FLAG_POSIX_SEMANTICS
    , RandomAccess      = FILE_FLAG_RANDOM_ACCESS
    , SessionName       = FILE_FLAG_SESSION_AWARE
    , SequentalScan     = FILE_FLAG_SEQUENTIAL_SCAN
    , WriteThrough      = FILE_FLAG_WRITE_THROUGH
};
using FileFlags = Utils::Flags<FileFlag>;

template<DesiredAccess desiredAccess>
auto createFile(  const std::filesystem::path & path
                , const ShareFlags shareMode
                , const CreateMode creationDisposition
                , const FileFlags  flags )
{
    const HANDLE handle = ::CreateFileW
    (
          path.c_str()
        , static_cast<DWORD>(desiredAccess)
        , static_cast<DWORD>(shareMode)
        , NULL //securityAttributes
        , static_cast<DWORD>(creationDisposition)
        , static_cast<DWORD>(flags)
        , NULL //templateFile
    );
    
    auto close = [](const HANDLE handle)
    {
        ::CloseHandle(handle);
    };
    using FileHandle = Handle<decltype(close)>;
    
    if(handle == INVALID_HANDLE_VALUE)
    {
        return Maybe<FileHandle>{OccurredError{}};
    }
    return Maybe<FileHandle>{FileHandle{handle, std::move(close)}};
}

template<DesiredAccess desiredAccess>
using MaybeFile = decltype(createFile<desiredAccess>(
      std::filesystem::path{}
    , ShareFlag::None
    , CreateMode::CreateAlways
    , FileFlag::Normal  ));

template<DesiredAccess desiredAccess>
using File = typename MaybeFile<desiredAccess>::Type;

using FileAccessAll         = File<DesiredAccess::GenericAll>;
using FileAccessExecute     = File<DesiredAccess::GenericExecute>;
using FileAccessRead        = File<DesiredAccess::GenericRead>;
using FileAccessWrite       = File<DesiredAccess::GenericWrite>;
using FileAccessReadWrite   = File<DesiredAccess::GenericReadWrite>;

template<typename File>
concept IsItFile = std::is_same_v<File, FileAccessAll>
                || std::is_same_v<File, FileAccessExecute>
                || std::is_same_v<File, FileAccessRead>
                || std::is_same_v<File, FileAccessWrite>
                || std::is_same_v<File, FileAccessReadWrite>;

template<typename File>
concept IsFileAllowRead =  std::is_same_v<File, FileAccessAll>
                        || std::is_same_v<File, FileAccessRead>
                        || std::is_same_v<File, FileAccessReadWrite>;
template<typename File>
concept IsFileAllowWrite = std::is_same_v<File, FileAccessAll>
                        || std::is_same_v<File, FileAccessWrite>
                        || std::is_same_v<File, FileAccessReadWrite>;
template<typename File>
concept IsFileAllowReadWrite = std::is_same_v<File, FileAccessAll>
                        || std::is_same_v<File, FileAccessRead>
                        || std::is_same_v<File, FileAccessWrite>
                        || std::is_same_v<File, FileAccessReadWrite>;

template<typename File>
constexpr DesiredAccess fileAccess() noexcept
{
    if constexpr (std::is_same_v<File, FileAccessAll>)
    {
        return DesiredAccess::GenericAll;
    }
    else if constexpr (std::is_same_v<File, FileAccessRead>)
    {
        return DesiredAccess::GenericRead;
    }
    else if constexpr (std::is_same_v<File, FileAccessWrite>)
    {
        return DesiredAccess::GenericWrite;
    }
    else if constexpr (std::is_same_v<File, FileAccessReadWrite>)
    {
        return DesiredAccess::GenericReadWrite;
    }
    else if constexpr (true)
    {
        static_assert(false, "");
    }
}

template<typename F>
constexpr DesiredAccess fileAccess(const F&) noexcept
{
    return fileAccess<F>();
}

template<typename F>
void closeFile(F& file) requires IsItFile<F>
{
    file = nullptr;
}


enum class FilePointerFrom: DWORD
{
      Begin   = FILE_BEGIN
    , Current = FILE_CURRENT
    , End     = FILE_END
};

template<typename F, typename I>
requires IsFileAllowReadWrite<F>
Maybe<CountOf<I>> setFilePointer( const F& file
                                , const DiffOf<I> offset
                                , const FilePointerFrom from = FilePointerFrom::Begin ) 
{
    const LARGE_INTEGER distance{.QuadPart = static_cast<ptrdiff_t>(Utils::offsetOf(offset))};
    LARGE_INTEGER newPointer{};
    const bool succeed = FALSE != ::SetFilePointerEx
    (
          file.get()
        , distance
        , &newPointer
        , static_cast<DWORD>(from)
    );
    if(!succeed)
    {
        return OccurredError{};
    }
    const CountOfBytes position = CountOfBytes{} + OneByte * newPointer.QuadPart;
    return Utils::countOf<I>(position);
}

template<typename F>
Maybe<CountOfBytes> getFilePointer(const F& file)
{
    return setFilePointer(file, OffsetOfBytes{}, FilePointerFrom::Current);
}

template<typename F>
requires IsFileAllowReadWrite<F>
Maybe<void> setFilePointerToBegin(const F& file)
{
    return setFilePointer(file, OffsetOfBytes{}, FilePointerFrom::Begin);
}

template<typename F>
Maybe<void> setFilePointerToEnd(const F& file)
{
    return static_cast<bool>(setFilePointer(file, OffsetOfBytes{}, FilePointerFrom::End));
}


template<typename Handle, typename I>
requires IsFileAllowRead<Handle> && std::is_trivially_copyable_v<I>
CountOf<I> fileRead(const Handle& file, I * const buffer_begin, const I * const buffer_end)
{
    CountOfBytes totalBytes{};
    const auto begin = reinterpret_cast<std::byte * >(buffer_begin);
    const auto end   = reinterpret_cast<const std::byte * >(buffer_end);
    for(std::byte * position = begin; position < end;)
    {
        DWORD actuallyRead = 0u;
        const BOOL succedded = ::ReadFile
        (
              file.get()
            , reinterpret_cast<void * >(position)
            , static_cast<DWORD>(end - position)
            , &actuallyRead
            , nullptr
        );

        if(succedded && 0u == actuallyRead)
        {
            break; // end of file
        }

        position += actuallyRead;
        totalBytes += OneByte * actuallyRead;

        if(!succedded)
        {
            break;
        }
    }
    return Utils::countOf<I>(totalBytes);
}

template<typename Handle, typename V>
requires IsFileAllowRead<Handle> && std::is_trivially_copyable_v<V>
CountOf<V> fileReadValue(const Handle& file, V& value)
{
    DWORD actuallyRead = 0u;
    ::ReadFile
    (
          file.get()
        , reinterpret_cast<void*>(&value)
        , static_cast<DWORD>(sizeof(value))
        , &actuallyRead
        , nullptr
    );
    return Utils::countOf<V>(CountOfBytes{} + OneByte * actuallyRead);
}

template<typename Handle, typename C>
auto fileReadData(const Handle& file, C& container)
{
    const auto begin = std::data(container);
    const auto end = begin + std::size(container);
    return fileRead(file, begin, end);
}


template<typename F, typename I>
requires IsFileAllowWrite<F> && std::is_trivially_copyable_v<I>
CountOf<I> fileWrite(const F& file, const I * const buffer_begin, const I * const buffer_end)
{
    CountOfBytes totalBytes;
    const auto begin = reinterpret_cast<const std::byte * >(buffer_begin);
    const auto end   = reinterpret_cast<const std::byte * >(buffer_end);
    for(const std::byte * position = begin; position < end;)
    {
        DWORD written = 0u;
        const BOOL succedded = ::WriteFile
        (
              file.get()
            , reinterpret_cast<const void * >(position)
            , static_cast<DWORD>(end - position)
            , &written
            , nullptr
        );
        position += written;
        totalBytes += OneByte * written;

        if(!succedded)
        {
            break;
        }
    }
    return Utils::countOf<I>(totalBytes);
}

template<typename F, typename V>
requires IsFileAllowWrite<F> && std::is_trivially_copyable_v<I>
CountOf<V> fileWriteValue(const F& file, const V& value)
{
    DWORD written = 0u;
    ::WriteFile
    (
          file.get()
        , reinterpret_cast<const void *>(value)
        , static_cast<DWORD>(sizeof(value))
        , &written
        , nullptr
    );
    return Utils::countOf<V>(CountOfBytes{} + OneByte * written);
}

template<typename Handle, typename C>
auto fileWriteData(const Handle& file, const C& container)
{
    const auto begin = std::data(container);
    const auto end = begin + std::size(container);
    return fileWrite(file, begin, end);
}


}
