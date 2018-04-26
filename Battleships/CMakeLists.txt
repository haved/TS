cmake_minimum_required (VERSION 3.4.3)
project (Battleships)
set(Battleships_VERSION_MAJOR 1)

file(GLOB MySources src/*.cpp)
add_executable(Battleships ${MySources})
include_directories(PUBLIC "${CMAKE_SOURCE_DIR}")

