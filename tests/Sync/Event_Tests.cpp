//
//  Copyright © 2021, Alexander Borisov,  https://github.com/SashaBorisov/CppWinApi
//

#include <gtest/gtest.h>

#include <WinApi/Sync/Event.h>

using namespace WinApi;

TEST(Sync_Event, CreateManual)
{
    auto maybeEvent = Sync::createManualEvent();
    ASSERT_TRUE(maybeEvent.okay()) << maybeEvent.message();
    Sync::ManualEvent event = std::move(maybeEvent).value();

    EXPECT_TRUE(Sync::resetEvent(event).okay());
    EXPECT_TRUE(Sync::setEvent(event).okay());
}

TEST(Sync_Event, CreateAuto)
{
    auto maybeEvent = Sync::createAutoEvent();
    ASSERT_TRUE(maybeEvent.okay()) << maybeEvent.message();
    Sync::AutoEvent event = std::move(maybeEvent).value();

    EXPECT_TRUE(Sync::resetEvent(event).okay());
    EXPECT_TRUE(Sync::setEvent(event).okay());
}
