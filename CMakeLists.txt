cmake_minimum_required(VERSION 4.2)
project(Drone)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(cc_-sdk/CC_SDK)


add_executable(Drone main.cpp
        src/websocket.cpp
        include/websocket.h)
target_include_directories(Drone PUBLIC ${CC_API_INC})
target_link_libraries(Drone PUBLIC CC_API)
