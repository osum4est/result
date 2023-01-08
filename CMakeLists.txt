cmake_minimum_required(VERSION 3.19)

option(RESULT_BUILD_TESTING "Build tests" OFF)

include(FetchContent)
FetchContent_Declare(
        backward-cpp
        GIT_REPOSITORY https://github.com/bombela/backward-cpp
        GIT_TAG v1.6
)

project(result)
set(TARGET_NAME ${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 20)

add_library(${TARGET_NAME} INTERFACE)
set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${TARGET_NAME} INTERFACE include)

FetchContent_MakeAvailable(backward-cpp)
target_include_directories(${TARGET_NAME} INTERFACE ${BACKWARD_INCLUDE_DIRS})

if ((CMAKE_PROJECT_NAME STREQUAL TARGET_NAME OR RESULT_BUILD_TESTING) AND BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()