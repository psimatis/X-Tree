cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

add_executable(interval_test interval_test.cpp)

target_link_libraries(interval_test gtest_main)
target_link_libraries(interval_test gmock_main)

add_test(interval_test interval_test)
