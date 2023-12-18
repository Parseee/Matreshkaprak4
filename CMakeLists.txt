cmake_minimum_required(VERSION 3.5.0)
project(matreshka_prac VERSION 0.1.0 LANGUAGES C CXX)

add_executable(matreshka_prac main.cpp io_kit.cpp lexem_analyze.cpp parse.cpp)

SET(CMAKE_CXX_FLAGS "-std=c++11")