cmake_minimum_required (VERSION 3.4.3)
project (Battleships)
set(Battleships_VERSION_MAJOR 1)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

option(USE_STDIO "Use stdin and stdout instead of /dev/ttyXXXX" OFF)
option(DEBUG_IO "Print all serial comms" OFF)

file(GLOB MySources src/*.cpp)
add_executable(Battleships ${MySources})
include_directories(PUBLIC "${CMAKE_SOURCE_DIR}")

find_package(PkgConfig REQUIRED)
pkg_search_module(SDL2_mixer REQUIRED SDL2_mixer)
INCLUDE_DIRECTORIES(${SDL2_mixer_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(Battleships ${SDL2_mixer_LIBRARIES})


SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

if(USE_STDIO)
  target_compile_definitions(Battleships PRIVATE "USE_STDIO")
endif()

if(DEBUG_IO)
  target_compile_definitions(Battleships PRIVATE "DEBUG_IO")
endif()
