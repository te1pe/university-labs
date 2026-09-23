cmake_minimum_required(VERSION 3.15)
project(flappy_min CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML REQUIRED COMPONENTS Graphics Window System)

file(GLOB SRC CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(flappy_min ${SRC})

target_include_directories(flappy_min PRIVATE
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/src"
)

target_link_libraries(flappy_min PRIVATE SFML::Graphics SFML::Window SFML::System)

# Кладём бинарь в build/bin
set_target_properties(flappy_min PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Копируем ассеты рядом с бинарём
add_custom_command(TARGET flappy_min POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:flappy_min>/assets"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_SOURCE_DIR}/assets"
          "$<TARGET_FILE_DIR:flappy_min>/assets"
  COMMENT "Copy assets to output directory"
)
