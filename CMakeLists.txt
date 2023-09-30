cmake_minimum_required(VERSION 3.26)
project(ByteFrost)

set(CMAKE_CXX_STANDARD 17)

add_library(ByteFrost library.cpp
        Server.cpp
        Server.h)

add_executable(ByteFrostChat chat.cpp)

target_link_libraries(ByteFrostChat ByteFrost)
