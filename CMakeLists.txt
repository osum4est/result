cmake_minimum_required(VERSION 3.19)

set(TARGET_NAME result)
project(${TARGET_NAME})

add_library(${TARGET_NAME} INTERFACE)

target_include_directories(${TARGET_NAME} INTERFACE include)
target_include_directories(${TARGET_NAME} INTERFACE external/backward-cpp)

set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)