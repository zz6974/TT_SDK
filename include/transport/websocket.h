#ifndef DRONE_WEBSOCKET_H
#define DRONE_WEBSOCKET_H

#include "WebSocket/CCWebSocket.h"
#include <thread>
#include <functional>

//传输层：只负责原始字节流的收发，不感知帧协议与业务

class websocket_server {
public:
    //二进制消息回调：上层（业务路由）通过它订阅原始字节流
    using OnBinaryMessage = std::function<void(CTL::ByteArray&)>;
    void SetOnBinary(OnBinaryMessage cb) { onBinary_ = std::move(cb); }

    void OnOpen(CTL::WebClient& client);
    void OnMessage(CTL::WebClient& client);
    void OnClose(CTL::WebClient& client);
    void OnError(CTL::WebClient& client);
    void OnPing(CTL::WebClient& client);
    void OnPong(CTL::WebClient& client);

    //绑定监听（非阻塞，仅 Init + InitBind）
    int Bind(std::string ip, int port);
    //启动事件循环（阻塞）
    int Run();
    //兼容旧接口：绑定后立即阻塞运行
    int Server_Bind_Demo(std::string ip, int port);
private:
    CTL::WebSocketServer server;
    OnBinaryMessage onBinary_;
};

class websocket_client {
public:
    void ClientOnOpen(CTL::WebSocketInfo& client);
    void ClientOnMessage(CTL::WebSocketInfo& client);
    void ClientOnClose(CTL::WebSocketInfo& client);
    void ClientOnError(CTL::WebSocketInfo& client);
    void ClientOnPing(CTL::WebSocketInfo& client);
    void ClientOnPong(CTL::WebSocketInfo& client);
private:
    CTL::WebSocketClient client;
};


#endif //DRONE_WEBSOCKET_H
