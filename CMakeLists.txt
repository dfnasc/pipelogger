project(pipelogger)

cmake_minimum_required(VERSION 3.13)

set(PL_SRC "main.c")

include_directories(".")

add_executable(pipelogger ${PL_SRC})
