#include "app/DroneApp.h"

bool DroneApp::Init(const std::string& ip, int port) {
    // 1. 基础设施层：数据库建表 + 缓存装载
    DataBase_t::init();

    // 2. 业务层：注册全部业务 handler 并挂接拆帧回调
    router_.RegisterHandlers();

    // 3. 传输层：绑定监听，二进制流量接入业务路由
    server_.SetOnBinary([this](CTL::ByteArray& data) {
        router_.Append(data);
    });
    if (server_.Bind(ip, port) != 0) {
        CTL::System::Println("[App] WebSocket 绑定失败 {}:{}", ip, port);
        return false;
    }
    CTL::System::Println("[App] 初始化完成，监听 {}:{}", ip, port);
    return true;
}

int DroneApp::Run() {
    return server_.Run();
}
