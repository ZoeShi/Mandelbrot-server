cmake_minimum_required(VERSION 3.11)
project(Mandelbrot_Server)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

add_subdirectory(Simple-Web-Server)

find_package(Boost 1.53.0 COMPONENTS system thread filesystem REQUIRED)
add_executable(Mandelbrot_Server main.cpp json.hpp Mandelbrot.cpp Mandelbrot.h)
target_link_libraries(Mandelbrot_Server ${Boost_LIBRARIES})
target_include_directories(Mandelbrot_Server PRIVATE ${Boost_INCLUDE_DIR})
#include_directories("cxxhttp/include")
#include_directories("cxxhttp/dependencies/libefgy/include/")
#include_directories("cxxhttp/dependencies/asio/asio/include/")
include_directories("Simple-Web-Server")
