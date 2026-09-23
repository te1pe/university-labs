cmake_minimum_required(VERSION 3.10)
project(lab_3)

set(CMAKE_CXX_STANDARD 17)

add_executable(lab_3 main.cpp converter.h converter_factory.h converter_mix.h converter_mute.h converter_loudly.h config_reader.h converter_mute.cpp converter_mix.cpp converter_loudly.cpp config_reader.cpp converter_factory.cpp wav_reader.h wav_reader.cpp wav_writer.h wav_writer.cpp)
