cmake_minimum_required(VERSION 2.8)

project(cHealpix)
enable_testing()

set(VERSION 0.1)

add_subdirectory(./src)
add_subdirectory(./test)
add_subdirectory(./pkgconfig)

