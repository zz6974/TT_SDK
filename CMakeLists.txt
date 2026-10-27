cmake_minimum_required(VERSION 4.2)
project(Drone)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(cc_-sdk/CC_SDK)


add_executable(Drone main.cpp
        src/websocket.cpp
        include/websocket.h
        src/FrameRoute.cpp
        include/FrameRoute.h
        src/Dispose.cpp
        src/comutil_shim.cpp
        include/Dispose.h
        include/sqlite.h
        src/DataBase.cpp
        include/DataBase.h
)

target_include_directories(Drone PUBLIC ${CC_API_INC})
target_link_libraries(Drone PUBLIC CC_API)
if(WIN32)
    target_link_libraries(Drone PUBLIC wbemuuid)
endif()
# 把工程根目录注入为字符串宏，源码用 PROJECT_ROOT_DIR 拼接数据库路径
target_compile_definitions(Drone PRIVATE PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")