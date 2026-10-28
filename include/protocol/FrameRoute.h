#ifndef DRONE_FRAMEROUTE_H
#define DRONE_FRAMEROUTE_H

#include "CCWebSocket.h"
#include "JSON/Json.hpp"
#include <cstdint>
#include <functional>
#include <thread>
//定义数据帧格式


//开头定位符
constexpr uint16_t FRAME_MAGIC = 0xCCDD;
constexpr uint32_t MAX_FRAME_LENGTH = 16 * 1024 * 1024;

enum FrameType : uint8_t{
    FT_UNKNOW = 0x00,
    //图片帧
    FT_VIDEO = 0x01,
    //传感器帧(文本帧)
    FT_SENSOR = 0x02,
    //控制指令
    FT_CONTROL = 0x03,
};

#pragma pack(push, 1)
struct FrameHeader {
    uint16_t magic;
    uint8_t  type;// FrameType
    uint32_t length;// length
};
#pragma pack(pop)

struct Frame {
    FrameType type = FT_UNKNOW;
    CTL::ByteArray payload;
};

// 组帧：给业务数据套上帧头（发送端）
CTL::ByteArray PackFrame(FrameType type, const CTL::ByteArray& payload);

//拆帧状态机：从原始字节流中解析出完整数据帧
class FrameParser    {
public:
    using OnFrame = std::function<void(const Frame&)>;
    explicit FrameParser(uint32_t maxFrameLength = MAX_FRAME_LENGTH);

    void SetOnFrame(OnFrame cb);

    //原始字节数据流入口：WebSocket/串口收到的数据直接传进来
    size_t Append(const CTL::ByteArray& data);

    // 原始指针版本（串口 readBuffer 等场景）
    size_t Append(const char* data, size_t size);

    //缓冲区剩余未解析的字节
    [[nodiscard]] size_t Remain() const;
    //清除
    void Reset();

    size_t TryParse();

private:
    CTL::ByteArray m_buffer;
    OnFrame onFrame_;
    uint32_t m_maxFrameLength_;
};

#endif //DRONE_FRAMEROUTE_H
