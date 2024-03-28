cmake_minimum_required(VERSION 3.7)
project(panzgb)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)
include_directories(panzgb ${SDL2_INCLUDE_DIRS})
add_executable(panzgb gb-memory.c gb-opcodes.c gb-opcodes-impl.c gb-sound.c gb-video.c gc-imp.c gc-interrupts.c mbc1.c mbc3.c panzgb.c)
target_link_libraries(panzgb ${SDL2_LIBRARIES})
