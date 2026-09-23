cmake_minimum_required(VERSION 3.16)
project(csvparser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(csvparser
  main.cpp
)

target_include_directories(csvparser PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(csvparser PRIVATE -Wall -Wextra -Wpedantic)
elseif (MSVC)
  target_compile_options(csvparser PRIVATE /W4 /permissive-)
endif ()

# Кладём пример данных рядом с бинарём
add_custom_command(TARGET csvparser POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
          "${CMAKE_CURRENT_SOURCE_DIR}/data.csv"
          "$<TARGET_FILE_DIR:csvparser>/data.csv"
  COMMENT "Copy sample data next to the binary"
)
