#include "protocol/FrameRoute.h"

#include <cstring>

// 组帧：给业务数据套上帧头（发送端）
CTL::ByteArray PackFrame(FrameType type, const CTL::ByteArray& payload) {
    FrameHeader header;
    header.magic  = FRAME_MAGIC;
    header.type   = static_cast<uint8_t>(type);
    header.length = static_cast<uint32_t>(payload.size());
    CTL::ByteArray frame(&header, sizeof(FrameHeader));   // 帧头
    frame.append(payload);                                // payload
    return frame;
}

FrameParser::FrameParser(uint32_t maxFrameLength) : m_maxFrameLength_(maxFrameLength) {}

void FrameParser::SetOnFrame(OnFrame cb) {
    onFrame_ = std::move(cb);
}

//原始字节数据流入口：WebSocket/串口收到的数据直接传进来
size_t FrameParser::Append(const CTL::ByteArray& data) {
    m_buffer.append(data);
    return TryParse();
}

// 原始指针版本（串口 readBuffer 等场景）
size_t FrameParser::Append(const char* data, size_t size) {
    m_buffer.append(CTL::ByteArray(data, size));
    return TryParse();
}

//缓冲区剩余未解析的字节
size_t FrameParser::Remain() const {
    return m_buffer.size();
}

//清除
void FrameParser::Reset() {
    m_buffer.clear();
}

size_t FrameParser::TryParse() {
    size_t count = 0;
    while (true) {
        const int pos = m_buffer.indexOf(CTL::ByteArray({0xCC, 0xDD}));
        if (pos < 0) {
            return count;
        }
        // 丢弃帧头之前的垃圾字节，从帧头开始对齐
        if (pos > 0) {
            m_buffer.poll(0, pos);
        }
        // 帧头尚未完整到达（半包），等待下一次 Append
        if (m_buffer.size() < sizeof(FrameHeader)) {
            return count;
        }

        FrameHeader hdr{};
        std::memcpy(&hdr, m_buffer.buffer(), sizeof(FrameHeader));
        if (hdr.length > m_maxFrameLength_) {
            m_buffer.poll(0, 1);
            continue;
        }

        const size_t total = sizeof(FrameHeader) + hdr.length;
        if (m_buffer.size() < total) return count;

        m_buffer.poll(0, sizeof(FrameHeader));
        Frame frame;
        frame.type = static_cast<FrameType>(hdr.type);
        frame.payload = m_buffer.poll(0, hdr.length);

        if (onFrame_) onFrame_(frame);
        ++count;
    }
    return count;
}
