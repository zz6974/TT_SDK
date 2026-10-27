#ifndef DRONE_FRAMEROUTE_H
#define DRONE_FRAMEROUTE_H

#include "CCWebSocket.h"
#include "JSON/Json.hpp"
#include<thread>
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

//组帧
// 组帧：给业务数据套上帧头（发送端）
inline CTL::ByteArray PackFrame(FrameType type, const CTL::ByteArray& payload) {
    FrameHeader header;
    header.magic  = FRAME_MAGIC;
    header.type   = static_cast<uint8_t>(type);
    header.length = static_cast<uint32_t>(payload.size());
    CTL::ByteArray frame(&header, sizeof(FrameHeader));   // 帧头
    frame.append(payload);                           // payload
    return frame;
}

struct Frame {
    FrameType type = FT_UNKNOW;
    CTL::ByteArray payload;
};

class FrameParser    {
public:
    using OnFrame = std::function<void(const Frame&)>;
    explicit FrameParser(uint32_t maxFrameLength = MAX_FRAME_LENGTH) : m_maxFrameLength_(maxFrameLength) {}

    void SetOnFrame(OnFrame cb) {onFrame_ = std::move(cb);}

    //原始字节数据流入口：WebSocket/串口收到的数据直接传进来
    size_t Append(const CTL::ByteArray& data) {
        m_buffer.append(data);
        return TryParse();
    }

    // 原始指针版本（串口 readBuffer 等场景）
    size_t Append(const char* data, size_t size) {
        m_buffer.append(CTL::ByteArray(data, size));
        return TryParse();
    }

    //缓冲区剩余未解析的字节
    [[nodiscard]] size_t TryParse() const{return m_buffer.size();}
    //清除
    void Reset(){m_buffer.clear();}
    size_t TryParse() {
        size_t count = 0;
        while (true) {
            const int pos = m_buffer.indexOf(CTL::ByteArray({0xCC, 0xDD}));
            if (pos < 0) {
                return count;
            }
            if (pos >= 0) {
                if (m_buffer.size() > 1) m_buffer.poll(0 ,m_buffer.size() - 1);
            }

            FrameHeader hdr{};
            std::memcpy(&hdr, m_buffer.buffer(), sizeof(FrameHeader));
            if (hdr.length > m_maxFrameLength_) {
                m_buffer.poll(0,1);
                continue;}

            const size_t total = sizeof(FrameHeader) + hdr.length;
            if (m_buffer.size() < total) return count;

            m_buffer.poll(0, sizeof(FrameHeader));
            Frame frame;
            frame.type = static_cast<FrameType>(hdr.type);
            frame.payload = m_buffer.poll(0, hdr.length);

            if (onFrame_) onFrame_(frame);
            // parser.SetOnFrame([&](const Frame& f) {
            // switch (f.type) {                        // ← 分流就在这
            //     case FT_SENSOR:  OnSensor(f.payload);  break;   // 传感器数据 → 传感器函数
            //     case FT_VIDEO:   OnVideo(f.payload);   break;   // 视频数据   → 视频函数
            //     case FT_CONTROL: OnControl(f.payload); break;   // 控制指令   → 控制函数
            //     default: break;
            //     }
            // });
            ++count;
        }
        return count;
    }
private:
    CTL::ByteArray m_buffer;
    OnFrame onFrame_;
    uint32_t m_maxFrameLength_;
};

#endif //DRONE_FRAMEROUTE_H
