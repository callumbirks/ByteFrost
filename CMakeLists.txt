cmake_minimum_required(VERSION 3.26)
project(ByteFrost)

set(CMAKE_CXX_STANDARD 17)

add_library(ByteFrost
        MessageServer.cpp
        Client.cpp
        DiscoveryServer.cpp
)

if (WIN32)
    target_link_libraries(ByteFrost wsock32 ws2_32)
endif ()

add_executable(ByteFrostChat chat.cpp)

target_link_libraries(ByteFrostChat ByteFrost)
