cmake_minimum_required(VERSION 3.19)

option(RESULT_BUILD_TESTING "Build tests" OFF)

project(result)
set(CMAKE_CXX_STANDARD 20)
set(TARGET_NAME ${PROJECT_NAME})

add_library(${TARGET_NAME} INTERFACE)

target_include_directories(${TARGET_NAME} INTERFACE include)
target_include_directories(${TARGET_NAME} INTERFACE external/backward-cpp)

set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)

if ((CMAKE_PROJECT_NAME STREQUAL TARGET_NAME OR RESULT_BUILD_TESTING) AND BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()