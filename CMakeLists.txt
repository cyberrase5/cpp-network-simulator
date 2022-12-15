cmake_minimum_required(VERSION 3.3)
project(Cpp-Network-Simulator)
include_directories("src")

#set teh name of the executable to be produced
set(EXECUTABLE_NAME Network)

#Add all .cpp files under src/ directory
file(GLOB SOURCES src/*.cpp)

add_executable(${EXECUTABLE_NAME} ${SOURCES})

set(CMAKE_BUILD_TYPE Debug)