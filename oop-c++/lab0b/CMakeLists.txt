cmake_minimum_required(VERSION 3.16)
project(csv_counter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(csv_counter
  main.cpp
  csv.cpp
)

target_include_directories(csv_counter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(csv_counter PRIVATE -Wall -Wextra -Wpedantic)
elseif (MSVC)
  target_compile_options(csv_counter PRIVATE /W4 /permissive-)
endif ()
