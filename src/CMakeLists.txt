cmake_minimum_required(VERSION 3.10)
project(src)

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

add_executable( servidor servidor.cc)
target_link_libraries( servidor ${OpenCV_LIBS} )