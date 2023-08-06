cmake_minimum_required(VERSION 3.18)

project(regex-engine)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_executable(match regexp.cpp)

target_compile_options(match PUBLIC -Wall -Wpedantic -Werror -Wextra
        -g -z relro
        -Werror=format-security -Wall -pipe
        -grecord-gcc-switches -fstack-protector-strong
        -fstack-clash-protection
        -fpic -shared -fpie -pie -fexceptions -fasynchronous-unwind-tables
        -Wall -Wextra -Wformat=2 -Wswitch-default -Wcast-align 
        -Wpointer-arith 
        -Winline -Wundef -Wcast-qual -Wshadow  
        -Wwrite-strings -Wconversion -Wunreachable-code 
        -Wstrict-aliasing=2
        -ffloat-store -fno-common -fstrict-aliasing 
        -lm -pedantic -O0 -ggdb3 -pg
        
    -Wall
    -Wextra
    -Wpedantic
    -Wcast-align
    -Wcast-qual
    -Wdisabled-optimization
    -Wduplicated-branches
    -Wduplicated-cond
    -Wformat=2
    -Wlogical-op
    -Wmissing-include-dirs
    -Wnull-dereference
    -Woverloaded-virtual
    -Wpointer-arith
    -Wshadow
    -Wswitch-enum
    -Wvla
    -Wconversion
    -std=c++17 -pedantic -Wall -Wextra -Werror -Wshadow -Wsign-conversion -g
    -g -std=c++17 -pedantic -Wall -Wextra -Werror -Wshadow -Wconversion -Wunreachable-code
)