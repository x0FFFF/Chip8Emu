cmake_minimum_required(VERSION 3.30)
project(Chip8Emu C)

set(CMAKE_C_STANDARD 11)

find_package(SDL2 REQUIRED)


add_executable(Chip8Emu main.c
        chip8.h
        chip8.c)

target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        SDL2::SDL2
        SDL2::SDL2main)