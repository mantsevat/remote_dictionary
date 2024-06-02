cmake_minimum_required(VERSION 3.14)

file(GLOB test_src
    "${PROJECT_SOURCE_DIR}/dictionary/*.h"
    "${PROJECT_SOURCE_DIR}/dictionary/*.cpp" dictionary_test.cc)

add_executable(unit_tests ${test_src})
target_link_libraries(unit_tests PUBLIC GTest::gtest_main
    spdlog::spdlog)

include(GoogleTest)
gtest_discover_tests(unit_tests)
