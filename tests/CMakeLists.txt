#
#  Copyright © 2021, Alexander Borisov,  https://github.com/SashaBorisov/CppWinApi
#

cmake_minimum_required(VERSION 3.14)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# enable_testing()


add_executable(CppWinApi_Tests 
./Utils/CountOf_Tests.cpp
./Utils/Mask_Tests.cpp
./IO/File_Tests.cpp
./Sync/Event_Tests.cpp
./Heap_Tests.cpp
)
target_include_directories(CppWinApi_Tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../iface)
target_link_libraries(CppWinApi_Tests gtest_main)

include(GoogleTest)
gtest_discover_tests(CppWinApi_Tests)


