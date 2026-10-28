#include "service/ServiceRouter.h"
#include "service/UserService.h"
#include "service/TelemetryService.h"
#include "JSON/Json.hpp"

//注册全部业务 handler 并挂接拆帧回调（应用层 Init 时调用一次）
void ServiceRouter::RegisterHandlers() {
    UserService user;
    TelemetryService telemetry;

    //控制大类（FT_CONTROL）
    Register("UserVerify", [user](const CTL::JsonObject& data) {
        user.UserVerify(data);
    });

    //传感器大类（FT_SENSOR）
    Register("GPSLocation", [telemetry](const CTL::JsonObject& data) {
        telemetry.GPSLocation(data);
    });
    Register("IMUData", [telemetry](const CTL::JsonObject& data) {
        telemetry.IMUData(data);
    });
    Register("BaroData", [telemetry](const CTL::JsonObject& data) {
        telemetry.BaroData(data);
    });
    Register("BatteryData", [telemetry](const CTL::JsonObject& data) {
        telemetry.BatteryData(data);
    });
    Register("MotorStatus", [telemetry](const CTL::JsonObject& data) {
        telemetry.MotorStatus(data);
    });
    Register("Accelerometer", [telemetry](const CTL::JsonObject& data) {
        telemetry.Accelerometer(data);
    });

    //拆帧回调：按 FrameType 大类分流
    parser_.SetOnFrame([this](const Frame& frame) {
        OnFrame(frame);
    });
}

//传输层原始字节入口
size_t ServiceRouter::Append(CTL::ByteArray& data) {
    return parser_.Append(data);
}

//注册单个业务动作
void ServiceRouter::Register(const std::string& type, Handler handler) {
    handlers_.insert(std::pair(type, handler));
}

//按 FrameType 大类分流（一级路由）
void ServiceRouter::OnFrame(const Frame& frame) {
    switch (frame.type) {
        case FT_CONTROL:    //控制指令（鉴权/控制）
        case FT_SENSOR:     //传感器遥测（JSON 文本帧）
            Dispatch(frame.payload);
            break;
        case FT_VIDEO:      //视频帧 → 未来 VideoService 独立通道
            CTL::System::Println("[Router] FT_VIDEO 帧 size:{}", frame.payload.size());
            break;
        default:
            CTL::System::Println("[Router] 未知帧类型:{}", static_cast<int>(frame.type));
            break;
    }
}

//JSON 信封解析 + 消息级分发（二级路由）：{"type":"...","data":{...}}
void ServiceRouter::Dispatch(const CTL::ByteArray& payload) {
    const std::string text = payload.toString();
    CTL::JsonObject json;
    try {
        json = CTL::JsonObject::parse(text);
    } catch (...) {
        CTL::System::Println("[Router] JSON 解析失败: {}", text);
        return;
    }
    if (!json.isObject() || !json.has("type") || !json.has("data")) {
        CTL::System::Println("[Router] 缺少信封字段 type/data: {}", text);
        return;
    }
    const std::string type = json.get("type").asString();
    if (!handlers_.exist(type)) {
        CTL::System::Println("[Router] 未注册的业务动作: {}", type);
        return;
    }
    handlers_[type](json.get("data"));
}
