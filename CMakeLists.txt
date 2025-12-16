cmake_minimum_required(VERSION 4.0)
project(mlib_flat_deque)

set(CMAKE_CXX_STANDARD 11)

add_executable(mlib_flat_deque main.cpp
        include/mlib/flat_deque/flat_deque.hpp)
