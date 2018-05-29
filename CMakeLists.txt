cmake_minimum_required(VERSION 3.11)
project(Mandelbrot_Server)

set(CMAKE_CXX_STANDARD 14)

add_executable(Mandelbrot_Server main.cpp json.hpp Mandelbrot.cpp Mandelbrot.h)
include_directories("cxxhttp/include")
include_directories("cxxhttp/dependencies/libefgy/include/")
include_directories("cxxhttp/dependencies/asio/asio/include/")
