#ifndef DRONE_SERVICEROUTER_H
#define DRONE_SERVICEROUTER_H

#include "protocol/FrameRoute.h"
#include "CCSystem.h"

//业务层：业务动作处理器，入参为 JSON 信封中的 data 对象
using Handler = std::function<void(const CTL::JsonObject&)>;

//业务层：消息级路由。
//订阅传输层字节流 → 拆帧 → 按 FrameType 大类 + JSON 信封 type 字段二级分发
class ServiceRouter {
public:
    //注册全部业务 handler（应用层 Init 时调用一次）
    void RegisterHandlers();
    //传输层原始字节入口（对应 websocket_server::SetOnBinary 回调）
    size_t Append(CTL::ByteArray& data);
    //注册单个业务动作（type 为 JSON 信封中的动作名）
    void Register(const std::string& type, Handler handler);
private:
    //拆帧回调：按 FrameType 大类分流
    void OnFrame(const Frame& frame);
    //JSON 信封解析 + 消息级分发：{"type":"...","data":{...}}
    void Dispatch(const CTL::ByteArray& payload);

    FrameParser parser_;
    CCMap<std::string, Handler> handlers_;
};

#endif //DRONE_SERVICEROUTER_H
