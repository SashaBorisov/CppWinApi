//
//  Copyright © 2021, Alexander Borisov,  https://github.com/SashaBorisov/CppWinApi
//

#include <gtest/gtest.h>

#include <WinApi/Heap.h>

using namespace WinApi;


TEST(Heap, Create)
{
    auto maybeHeap = createHeap<HeapFlags::NoSerialize, int>();
    ASSERT_TRUE(maybeHeap.okay()) << maybeHeap.message();
    auto heap = std::move(maybeHeap).value();

    // auto instance = heapEmplace(heap, 137);

}