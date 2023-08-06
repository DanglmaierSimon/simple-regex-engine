cmake_minimum_required(VERSION 3.18)

project(regex-engine)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_library(regex SHARED regexp.h regexp.cpp)

add_executable(match main.cpp)
target_link_libraries(match PRIVATE regex)


target_compile_options(match PUBLIC
    -fasynchronous-unwind-tables
    -fexceptions
    -ffloat-store
    -fno-common
    -fpic
    -fpie
    -fstack-clash-protection
    -fstack-protector-strong
    -fstrict-aliasing
    -g
    -ggdb3
    -grecord-gcc-switches
    -lm
    -O0
    -pedantic
    -pg
    -pie
    -pipe
    -shared
    -std=c++17
    -Wall
    -Wcast-align
    -Wcast-qual
    -Wconversion
    -Wdisabled-optimization
    -Wduplicated-branches
    -Wduplicated-cond
    -Werror
    -Werror=format-security
    -Wextra
    -Wformat=2
    -Winline
    -Wlogical-op
    -Wmissing-include-dirs
    -Wnull-dereference
    -Woverloaded-virtual
    -Wpedantic
    -Wpointer-arith
    -Wshadow
    -Wsign-conversion
    -Wstrict-aliasing=2
    -Wswitch-default
    -Wswitch-enum
    -Wundef
    -Wunreachable-code
    -Wvla
    -Wwrite-strings
)