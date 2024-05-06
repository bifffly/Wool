cmake_minimum_required(VERSION 3.27)
project(Wool C)

set(CMAKE_C_STANDARD 99)

add_executable(wool src/main.c src/common.h
    src/chunk.h src/chunk.c
    src/memory.h src/memory.c
    src/debug.h src/debug.c
    src/value.h src/value.c
    src/vm.h src/vm.c
    src/compiler.h src/compiler.c
    src/scanner.h src/scanner.c
    src/ref.h src/ref.c
    src/refstring.h src/refstring.c
    src/table.h src/table.c)