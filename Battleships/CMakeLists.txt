cmake_minimum_required (VERSION 3.4.3)
project (Battleships)
set(Battleships_VERSION_MAJOR 1)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

option(USE_STDIO "Use stdin and stdout instead of /dev/ttyXXXX" OFF)

file(GLOB MySources src/*.cpp)
add_executable(Battleships ${MySources})
include_directories(PUBLIC "${CMAKE_SOURCE_DIR}")

if(USE_STDIO)
  target_compile_definitions(Battleships PRIVATE "USE_STDIO")
endif()
