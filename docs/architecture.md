# Drone 分层架构设计

## 1. 分层总览

程序采用五层架构，上层只依赖下层，禁止跨层反向依赖：

```
┌────────────────────────────────────────────────────────────┐
│ 应用层   app/DroneApp          装配与启动，唯一知晓全部层级      │
├────────────────────────────────────────────────────────────┤
│ 业务层   service/              业务动作路由与处理（鉴权/遥测）    │
├────────────────────────────────────────────────────────────┤
│ 协议层   protocol/FrameRoute   帧编解码（PackFrame/FrameParser）│
├────────────────────────────────────────────────────────────┤
│ 传输层   transport/websocket   原始字节流收发                  │
├────────────────────────────────────────────────────────────┤
│ 基础设施层 infra/              DB 实体与缓存（sqlite/DataBase） │
└────────────────────────────────────────────────────────────┘
```

依赖规则：

| 层 | 可以依赖 |
|---|---|
| app（应用层） | service、protocol、transport、infra |
| service（业务层） | protocol、infra |
| protocol（协议层） | 无（仅 SDK 与标准库） |
| transport（传输层） | 无（仅 SDK 与标准库） |
| infra（基础设施层） | 无（仅 SDK 与标准库） |

## 2. 目录结构

```
D:\Project\Drone\
├── main.cpp                        # 入口：仅装配与启动
├── CMakeLists.txt                  # 按层分组声明源文件
├── docs\architecture.md            # 本文档
├── include\
│   ├── app\DroneApp.h              # 应用层
│   ├── service\
│   │   ├── ServiceRouter.h         # 业务层：消息级路由
│   │   ├── UserService.h           # 业务层：用户鉴权
│   │   └── TelemetryService.h      # 业务层：遥测数据
│   ├── protocol\FrameRoute.h       # 协议层：帧格式与解析
│   ├── transport\websocket.h       # 传输层：WebSocket 封装
│   └── infra\
│       ├── sqlite.h                # 基础设施层：EntityBase CRTP 缓存基类
│       └── DataBase.h              # 基础设施层：实体定义 + DataBase_t
└── src\                            # 与 include\ 一一对应的实现
    ├── app\DroneApp.cpp
    ├── service\{ServiceRouter,UserService,TelemetryService}.cpp
    ├── protocol\FrameRoute.cpp
    ├── transport\websocket.cpp
    └── infra\DataBase.cpp
```

头文件统一以 `<层名>/<文件名>` 形式引用（如 `#include "protocol/FrameRoute.h"`），
由 CMake 的 `target_include_directories(Drone PUBLIC ${CMAKE_SOURCE_DIR}/include)` 支撑。

## 3. 各层职责

### 3.1 基础设施层（infra/）
- `EntityBase<Derived>`（sqlite.h）：CRTP 基类，提供每实体的内存缓存（线程安全 Map）
  与 `Add/Update/Remove/Get/GetAll/GetID` 静态操作。
- `DataBase.h`：实体定义（User/GPSData/IMUData/BatteryData/BaroData）与
  `DataBase_t::init()`（建表 + 缓存装载）。数据库路径用 `PROJECT_ROOT_DIR` 宏
  拼接为绝对路径，避免运行目录不同导致错位。
- 新增实体只需：定义实体类 + `PERSISTENT_INTRUSIVE` 注册字段 +
  在 `DataBase_t::init()` 中加一行 `InitTable<T>()`。

### 3.2 传输层（transport/）
- 只负责原始字节流收发，不感知帧协议与业务。
- `websocket_server::SetOnBinary`：上层订阅二进制消息回调；
  `Bind()` 绑定监听（非阻塞），`Run()` 启动事件循环（阻塞），
  `Server_Bind_Demo()` 为兼容旧接口的绑定+阻塞组合。
- 未来接入串口/RTSP 时，新增传输实现类即可，不影响上层。

### 3.3 协议层（protocol/）
- 帧格式：`magic(2B, 0xCCDD) + type(1B) + length(4B) + payload(length B)`，
  无帧尾，凭 `length` 定界；帧头 7 字节，`#pragma pack(1)` 对齐。
- `PackFrame`：组帧（发送端）；`FrameParser`：拆帧状态机（接收端），
  累积缓冲区处理粘包/半包，通过 `SetOnFrame` 回调输出 `Frame{type, payload}`。
- 不解析 payload 内容，不感知业务语义。

### 3.4 业务层（service/）
- `ServiceRouter`：消息级路由中枢。`Append()` 接收传输层字节流，
  `FrameParser` 拆帧后按 `FrameType` 大类分流（一级路由），
  再解析 payload 内 JSON 信封 `{"type":"...","data":{...}}` 的 `type` 字段
  分发到注册的 handler（二级路由）。
- `UserService`：鉴权业务（`UserVerify`），通过 infra 的 `User` 实体校验。
- `TelemetryService`：遥测业务（GPS/IMU/气压/电池/电机/加速度），
  `GPSLocation` 为入库参考实现，其余待补全。
- 扩展点：
  - `FT_CONTROL` → 鉴权与控制类 handler
  - `FT_SENSOR` → 遥测类 handler
  - `FT_VIDEO` → 未来 VideoService（独立通道，不走 JSON 信封）

### 3.5 应用层（app/）
- `DroneApp::Init()`：按序装配 ① 基础设施 `DataBase_t::init()`
  ② 业务 `ServiceRouter::RegisterHandlers()` ③ 传输层 `Bind()` 并接线
  `SetOnBinary → router_.Append`。
- `DroneApp::Run()`：启动传输层事件循环（阻塞）。
- `main.cpp` 仅负责创建 `DroneApp` 并调用 Init/Run。

## 4. 数据流

接收（上行）：

```
WebSocket 二进制 → transport.OnMessage → SetOnBinary 回调
→ ServiceRouter::Append → FrameParser 拆帧（粘包/半包处理）
→ FrameType 大类分流 → JSON 信封解析 → handler(type)
→ infra 实体读写（缓存 + SQLite）
```

发送（下行）：

```
handler 产出 JSON payload → PackFrame(type, payload) 组帧
→ transport.SendBinary → WebSocket 客户端
```

## 5. 两层路由机制

延续项目既有实践，业务分发采用两层路由：

1. **连接级路由**：WebSocket 握手 URL（如 `ws`）区分通道用途；
2. **消息级路由**：长连接内两级分发——
   - 帧头 `type`（FrameType）区分流量大类（控制/遥测/视频）；
   - payload 内 JSON 信封 `type` 字段区分具体业务动作（如 `UserVerify`）。

高频流量业务（视频）建议使用独立通道，避免与 JSON 遥测互扰。

## 6. 新增业务动作的步骤

1. 在 `service/` 下相应 Service 类中添加处理函数（入参 `const CTL::JsonObject&`）。
2. 在 `ServiceRouter::RegisterHandlers()` 中 `Register("动作名", lambda)`。
3. 如需持久化：在 `infra/DataBase.h` 定义实体并注册宏，
   在 `DataBase_t::init()` 加 `InitTable<T>()`。
