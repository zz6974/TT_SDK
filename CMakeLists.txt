cmake_minimum_required(VERSION 4.2)
project(Drone)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(cc_-sdk/CC_SDK)


add_executable(Drone main.cpp
        # 应用层
        src/app/DroneApp.cpp
        include/app/DroneApp.h
        # 业务层
        src/service/ServiceRouter.cpp
        include/service/ServiceRouter.h
        src/service/UserService.cpp
        include/service/UserService.h
        src/service/TelemetryService.cpp
        include/service/TelemetryService.h
        # 协议层
        src/protocol/FrameRoute.cpp
        include/protocol/FrameRoute.h
        # 传输层
        src/transport/websocket.cpp
        include/transport/websocket.h
        # 基础设施层
        src/infra/DataBase.cpp
        include/infra/DataBase.h
        include/infra/sqlite.h
        src/comutil_shim.cpp
)

# 分层头文件统一用 <层名>/<文件名> 形式引用
target_include_directories(Drone PUBLIC ${CC_API_INC} ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(Drone PUBLIC CC_API)
if(WIN32)
    target_link_libraries(Drone PUBLIC wbemuuid)
endif()
# 把工程根目录注入为字符串宏，源码用 PROJECT_ROOT_DIR 拼接数据库路径
target_compile_definitions(Drone PRIVATE PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")
