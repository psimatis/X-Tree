cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Interval test
add_executable(interval_test interval_test.cpp)
target_link_libraries(interval_test gtest_main)
target_link_libraries(interval_test gmock_main)
add_test(interval_test interval_test)

# Hyperrectangle test
add_executable(hyperrectangle_test hyperrectangle_test.cpp)
target_link_libraries(hyperrectangle_test gtest_main)
target_link_libraries(hyperrectangle_test gmock_main)
add_test(hyperrectangle_test hyperrectangle_test)

# XTree test
add_executable(xtree_test xtree_test.cpp)
target_link_libraries(xtree_test gtest_main)
target_link_libraries(xtree_test gmock_main)
add_test(xtree_test xtree_test)
