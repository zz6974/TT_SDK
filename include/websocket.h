#ifndef DRONE_WEBSOCKET_H
#define DRONE_WEBSOCKET_H

#include "WebSocket/CCWebSocket.h"
#include <thread>


class websocket_server {
public:
    void OnOpen(CTL::WebClient& client);
    void OnMessage(CTL::WebClient& client);
    void OnClose(CTL::WebClient& client);
    void OnError(CTL::WebClient& client);
    void OnPing(CTL::WebClient& client);
    void OnPong(CTL::WebClient& client);
    int Server_Bind_Demo(std::string ip, int port);
private:
    CTL::WebSocketServer server;
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
