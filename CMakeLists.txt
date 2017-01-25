cmake_minimum_required(VERSION 3.6)
project(JNP7)

set(CMAKE_CXX_STANDARD 14)

SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")

set(SOURCE_FILES tree.h)
add_executable(JNP7 ${SOURCE_FILES} tree_example.cc)