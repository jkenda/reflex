cmake_minimum_required(VERSION 3.10)
project(reflex)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

add_executable(test test.cpp)
target_include_directories(test PRIVATE src)
target_link_libraries(test ${GTEST_LIBRARIES})
target_compile_options(test PRIVATE -g -Wall -Wextra -Werror)
