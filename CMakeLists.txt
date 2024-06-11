cmake_minimum_required(VERSION 3.1)

project(NPC)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_BUILD_TYPE Debug)

find_package(Raylib REQUIRED)

add_executable(NPC main.cpp)

target_link_libraries(NPC PRIVATE -lraylib)
