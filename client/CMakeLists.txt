cmake_minimum_required(VERSION 3.0)
project(client)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost REQUIRED COMPONENTS system)
find_package(PkgConfig REQUIRED)
pkg_check_modules(EVDEV REQUIRED libevdev)

include_directories(${Boost_INCLUDE_DIRS} ${EVDEV_INCLUDE_DIRS})

add_executable(client src/main.cpp)
target_link_libraries(client ${Boost_LIBRARIES} ${EVDEV_LIBRARIES})
