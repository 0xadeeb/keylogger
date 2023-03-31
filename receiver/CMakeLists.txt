cmake_minimum_required(VERSION 3.0)
project(receiver)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost REQUIRED COMPONENTS system)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(receiver src/main.cpp)
target_link_libraries(receiver ${Boost_LIBRARIES})
