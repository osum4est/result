set(TARGET_NAME ${PROJECT_NAME}-tests)

add_executable(${TARGET_NAME} result.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE ${PROJECT_NAME})

find_package(Catch2 3 REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE Catch2::Catch2WithMain)
include(CTest)
include(Catch)
catch_discover_tests(${TARGET_NAME})