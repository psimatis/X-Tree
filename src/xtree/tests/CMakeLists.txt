cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

add_executable(xtree_test xtree_test.cpp)

target_link_libraries(xtree_test gtest_main)
target_link_libraries(xtree_test gmock_main)

add_test(xtree_test xtree_test)
