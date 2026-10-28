#include "app/DroneApp.h"

//入口：仅负责装配与启动，业务逻辑全部位于各层内部
int main() {
    DroneApp app;
    if (!app.Init("0.0.0.0", 9000)) {
        return -1;
    }
    return app.Run();
}
