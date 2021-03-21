cmake_minimum_required(VERSION 3.10)

project(fmt)

include_directories($(CMAKE_CURRENT_SOURCE_DIR)/fmt/include)

add_executable(fmt main.cpp fmt/src/format.cc fmt/src/os.cc)