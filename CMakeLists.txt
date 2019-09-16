cmake_minimum_required(VERSION 3.6)

set(SOURCE_FILES main.c util.c)
set(HEADER_FILES util.h)
set(CMAKE_C_FLAGS "-lGL -lglut -lGLEW -lm")

add_executable(Main ${SOURCE_FILES})