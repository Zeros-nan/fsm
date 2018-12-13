cmake_minimum_required(VERSION 3.0)
project(fsm)

set(CMAKE_CXX_STANDARD 11)

file(GLOB CPP_FILES *.cc)

add_executable(${PROJECT_NAME} ${CPP_FILES})


