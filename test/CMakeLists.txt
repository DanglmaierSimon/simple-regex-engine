# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(
  regex-test
  regex-test.cpp
)
target_link_libraries(
  regex-test
  GTest::gtest_main
  regex
)

target_include_directories(regex-test PRIVATE ../lib)

include(GoogleTest)
gtest_discover_tests(regex-test)