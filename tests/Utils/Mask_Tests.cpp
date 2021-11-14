//
//  Copyright © 2021, Alexander Borisov,  https://github.com/SashaBorisov/CppWinApi
//
#include <gtest/gtest.h>

#include <Utils/Mask.h>

using namespace Utils;

TEST(Mask, UniqueTag)
{
    using Flag1 = Flag<1, UNIQUE_TAG>;
    using Flag2 = Flag<1, UNIQUE_TAG>;
    static_assert(!std::is_same_v<Flag1, Flag2>);

    using FlagTrue1 = Flag<true, UNIQUE_TAG>;
    using FlagTrue2 = Flag<true, UNIQUE_TAG>;
    static_assert(!std::is_same_v<FlagTrue1, FlagTrue2>);
}

struct TestEnum
{
    using Type = int;
    static constexpr Flag<0,TestEnum> Flag0{};
    static constexpr Flag<1,TestEnum> Flag1{};
    static constexpr Flag<2,TestEnum> Flag2{};
    static constexpr Flag<4,TestEnum> Flag4{};
};

TEST(Mask, Declaration)
{
    using TestMask = Mask<TestEnum>;

    constexpr TestMask actual0 = TestEnum::Flag0 | TestEnum::Flag4;
    static_assert(std::is_same_v<const TestMask, decltype(actual0)>);
    
    constexpr auto actual1 = TestEnum::Flag1 | TestEnum::Flag2;
    static_assert(std::is_same_v<decltype(actual0), decltype(actual1)>);
}