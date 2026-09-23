cmake_minimum_required(VERSION 3.16)
project(life LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(life
  main.cpp
  lifem.cpp
  lifev.cpp
  lifec.cpp
)

target_include_directories(life PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(life PRIVATE -Wall -Wextra -Wpedantic)
elseif (MSVC)
  target_compile_options(life PRIVATE /W4 /permissive-)
endif ()

set_target_properties(life PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Копируем вселенные рядом с бинарём
add_custom_command(TARGET life POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:life>/universes"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_CURRENT_SOURCE_DIR}/universes"
          "$<TARGET_FILE_DIR:life>/universes"
  COMMENT "Copy universes to output directory"
)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(run_tests
  test.cpp
  lifem.cpp
)

target_include_directories(run_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(run_tests GTest::gtest_main)

set_target_properties(run_tests PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(run_tests)
