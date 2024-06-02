cmake_minimum_required(VERSION 3.14)

set(test_src
    dictionary_test.cc)

add_executable(unit_tests ${test_src})
target_link_libraries(unit_tests PUBLIC
    GTest::gtest_main
    test_lib)

include(GoogleTest)
gtest_discover_tests(unit_tests)
