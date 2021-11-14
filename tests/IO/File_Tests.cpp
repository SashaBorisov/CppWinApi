//
//  Copyright © 2021, Alexander Borisov,  https://github.com/SashaBorisov/CppWinApi
//

#include <gtest/gtest.h>

#include <WinApi/IO/File.h>

using namespace WinApi;

TEST(IO_File, SimpleReadWrite)
{
    // IO::doCreateFile()
    auto maybeFile = IO::createFile<IO::DesiredAccess::GenericReadWrite>
    (
          "test"
        , IO::ShareFlag::None
        , IO::CreateMode::CreateAlways
        , IO::FileFlag::Temporary | IO::FileFlag::DeleteOnClose
    );
    ASSERT_TRUE(maybeFile.okay()) << maybeFile.message();
    auto file = std::move(maybeFile).value();
    // static_assert(fileAccess(file) == IO::DesiredAccess::GenericReadWrite, "");

    const std::string expectedText = "expected text";
    const auto written = IO::fileWriteData(file, expectedText);
    ASSERT_EQ(Utils::countOf(expectedText), written);

    ASSERT_TRUE(IO::setFilePointerToBegin(file).okay()) << WinApi::lastErrorMessage();

    std::string actualText(expectedText.size(), '\0');
    const auto read = IO::fileReadData(file, actualText);
    ASSERT_EQ(written, read);

    ASSERT_EQ(expectedText, actualText);

    IO::closeFile(file);
}