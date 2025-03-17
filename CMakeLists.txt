cmake_minimum_required(VERSION 3.10)
project(MybankSystem)

set(CMAKE_CXX_STANDARD 17)
#aggiungere google test
#include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
        https://github.com/emmanuelnya/MybankSystems.git
)
FetchContent_MakeAvailable(googletest)
enable_testing()
add_executable(tests test_main.cpp)
target_link_libraries(tests gtest_main)
