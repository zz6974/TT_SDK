#ifndef DRONE_DRONEAPP_H
#define DRONE_DRONEAPP_H

#include "transport/websocket.h"
#include "service/ServiceRouter.h"
#include "infra/DataBase.h"

//应用层：装配与启动，是唯一知晓全部层级的地方
//依赖方向：app → service → protocol → transport，infra 被 service 与 app 使用
class DroneApp {
public:
    //初始化：基础设施建表 → 业务 handler 注册 → 传输层绑定
    bool Init(const std::string& ip, int port);
    //启动事件循环（阻塞）
    int Run();
private:
    websocket_server server_;   //传输层
    ServiceRouter router_;      //业务层
};

#endif //DRONE_DRONEAPP_H
