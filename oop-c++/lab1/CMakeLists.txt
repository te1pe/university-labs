cmake_minimum_required(VERSION 3.10)
project(CacheTests)

set(CMAKE_CXX_STANDARD 17)

add_executable(run_tests test.cpp ICACHELRU.cpp ICACHELFU.cpp fibonachi.cpp)

add_subdirectory(external/googletest)

target_link_libraries(run_tests gtest gtest_main)
