cmake_minimum_required(VERSION 3.18.5)

project("X tree")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(googletest)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

enable_testing()

# add_subdirectory(src/interval)
# add_subdirectory(src/hyperrectangle)
# add_subdirectory(src/xtree)
add_subdirectory(tests)

add_executable(x_tree src/main.cpp)
