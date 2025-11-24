cmake_minimum_required(VERSION 3.7)
project(panzgb)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Make static library with pangb core
ADD_LIBRARY( panzgb-core STATIC
             lib/gb-memory.c
             lib/gb-opcodes.c
             lib/gb-opcodes-impl.c
             lib/gb-sound.c
             lib/gb-video.c
             lib/gc-imp.c
             lib/gc-interrupts.c
             lib/mbc1.c
             lib/mbc3.c )
# PC clients
find_package(SDL2 REQUIRED)
include_directories(panzgb ${SDL2_INCLUDE_DIRS})
add_executable(panzgb panzgb.c)
target_link_libraries(panzgb ${SDL2_LIBRARIES} panzgb-core)
