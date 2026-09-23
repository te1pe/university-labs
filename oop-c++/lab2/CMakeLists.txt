cmake_minimum_required(VERSION 3.16)
project(bitarray LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(bitarray
  main.cpp
  bitarr.cpp
)

target_include_directories(bitarray PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(bitarray PRIVATE -Wall -Wextra -Wpedantic)
elseif (MSVC)
  target_compile_options(bitarray PRIVATE /W4 /permissive-)
endif ()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(run_tests
  test.cpp
  bitarr.cpp
)

target_include_directories(run_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(run_tests GTest::gtest_main)

enable_testing()
include(GoogleTest)
gtest_discover_tests(run_tests)
