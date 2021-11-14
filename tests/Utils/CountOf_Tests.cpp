//
//  Copyright © 2021, Alexander Borisov,  https://github.com/SashaBorisov/CppWinApi
//
#include <gtest/gtest.h>

#include <Utils/CountOf.h>

using namespace Utils;

TEST(Utils_CountOf, First)
{
    const char text[] = "test";
    constexpr auto charsCount  = Utils::countOf(text);
    constexpr auto sizeOfText1 = Utils::sizeOf(text);
    constexpr auto sizeOfText2 = Utils::sizeOf(charsCount);
    static_assert(std::is_same_v<decltype(sizeOfText1), decltype(sizeOfText2)>, "");
    ASSERT_EQ(sizeOfText1, sizeOfText2);

    const int numbers[5] = {0};
    constexpr auto intsCount      = Utils::countOf(numbers);
    constexpr auto sizeOfNumners1 = Utils::sizeOf(numbers);
    constexpr auto sizeOfNumners2 = Utils::sizeOf(intsCount);
    static_assert(std::is_same_v<decltype(sizeOfNumners1), decltype(sizeOfNumners2)>, "");
    ASSERT_EQ(sizeOfNumners1, sizeOfNumners2);

    static_assert(!std::is_same_v<decltype(charsCount), decltype(intsCount)>, "");
}