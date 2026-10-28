#include "transport/websocket.h"
#include "CCWebSocket.h"

void websocket_server::OnOpen(CTL::WebClient &client) {
    CTL::System::Println("OnOpen ID:{}" , client.GetID());

    const auto fds = server.getClientFds();

    for (const auto fd : fds) {
        CTL::WebClient* c = server.getClient(fd);
        if (c && c->id != client.id) {
            c->SendText("New client joined");
        }
    }
}

void websocket_server::OnMessage(CTL::WebClient &client) {
    auto packet = client.GetPacket();
    if (packet.get_opcode() == WebSocketPacket::WSOpcode_Text) {
        CTL::System::Println("OnMessage ID:{} {}", client.GetID(), "Text Message");
        const auto text = client.GetBuffer().toString();
        //回显给发送者
        client.SendText(text);
        //向客户端广播
        const auto fds = server.getClientFds();
        for (const auto fd : fds) {
            CTL::WebClient* c = server.getClient(fd);
            if (c && c->id != client.id) {
                c->SendText(text);
            }
        }
    }else if (packet.get_opcode() == WebSocketPacket::WSOpcode_Binary) {
        CTL::System::Println("OnMessage ID:{} {}", client.GetID(), "Binary Message");
        CTL::ByteArray binary = client.GetBuffer();
        //二进制流量交给上层订阅者处理，未订阅则回显保持兼容
        if (onBinary_) {
            onBinary_(binary);
        } else {
            client.SendBinary(binary);
        }
    }
}

inline void websocket_server::OnClose(CTL::WebClient &client) {
    CTL::System::Println("OnClose ID:{}" , client.GetID());
}

void websocket_server::OnError(CTL::WebClient &client) {
    CTL::System::Println("OnError ID:{}" , client.GetID());
}

void websocket_server::OnPing(CTL::WebClient &client) {
    CTL::System::Println("OnPing ID:{}" , client.GetID());

}

void websocket_server::OnPong(CTL::WebClient &client) {
    CTL::System::Println("OnPong ID:{}" , client.GetID());
}

//绑定监听：Init + InitBind，不阻塞
int websocket_server::Bind(std::string ip, int port) {
    if (!server.Init(ip, port)) {
        CTL::System::Println("[Server] Init failed");
        return -1;
    }
    CTL::WSBind bind;
    bind.URL = "/ws";
    bind.OnCloseFun = [this](CTL::WebClient& client) { OnClose(client); };
    bind.OnErrorFun = [this](CTL::WebClient& client) { OnError(client); };
    bind.OnMessageFun = [this](CTL::WebClient& client) { OnMessage(client); };
    bind.OnOpenFun = [this](CTL::WebClient& client) { OnOpen(client); };
    bind.OnPingFun = [this](CTL::WebClient& client) { OnPing(client); };
    bind.OnPongFun = [this](CTL::WebClient& client) { OnPong(client); };
    server.InitBind(bind);
    return 0;
}

//启动事件循环（阻塞）
int websocket_server::Run() {
    return server.Running();
}

//兼容旧接口：绑定 + 立即阻塞运行
int websocket_server::Server_Bind_Demo(std::string ip, int port) {
    if (Bind(ip, port) != 0) {
        return -1;
    }
    return Run();
}

void websocket_client::ClientOnOpen(CTL::WebSocketInfo &client) {
    CTL::System::Println("ClientOnOpen ID:{}" , client.GetID());
    client.SendText("Hello Server");
    client.SendBinary(CTL::ByteArray("ABC", 3));
}

void websocket_client::ClientOnMessage(CTL::WebSocketInfo &client) {
    auto packet = client.GetPacket();
    if (packet.get_opcode() == WebSocketPacket::WSOpcode_Text) {
        CTL::System::Println("ClientOnMessage ID:{} {}", client.GetID(), "Text Message");
        const auto text = client.GetBuffer().toString();
        CTL::System::Println("ClientOnMessage ID:{} {}", client.GetID(), text);
    } else if (packet.get_opcode() == WebSocketPacket::WSOpcode_Binary) {
        CTL::System::Println("ClientOnMessage ID:{} {}", client.GetID(), "Binary Message");
        const auto binary = client.GetBuffer();
        CTL::System::Println("ClientOnMessage ID:{} {}", client.GetID(), binary.toString());
    }
}

void websocket_client::ClientOnClose(CTL::WebSocketInfo &client) {
    CTL::System::Println("ClientOnClose ID:{}" , client.GetID());
}

void websocket_client::ClientOnError(CTL::WebSocketInfo &client) {
    CTL::System::Println("ClientOnError ID:{}" , client.GetID());
}

void websocket_client::ClientOnPing(CTL::WebSocketInfo &client) {
    CTL::System::Println("ClientOnPing ID:{}" , client.GetID());
    client.SendPong(CTL::ByteArray());
}

void websocket_client::ClientOnPong(CTL::WebSocketInfo &client) {
    CTL::System::Println("ClientOnPong ID:{}" , client.GetID());
}


