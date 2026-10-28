#ifndef DRONE_TELEMETRYSERVICE_H
#define DRONE_TELEMETRYSERVICE_H

#include "CCSystem.h"
#include "JSON/Json.hpp"

//业务层：遥测数据服务（FT_SENSOR 大类）
class TelemetryService {
public:
    //GPS定位：data = {"ts":int,"lat":double,"lng":double,"alt":double,"fix":int,"sat":int}
    void GPSLocation(const CTL::JsonObject& data) const;
    //IMU数据：data = {"ts":int,"ax":double,"ay":double,"az":double,"gx":double,"gy":double,"gz":double}
    void IMUData(const CTL::JsonObject& data) const;
    //气压计数据：data = {"ts":int,"pressure":double,"altitude":double,"temp":double}
    void BaroData(const CTL::JsonObject& data) const;
    //电池数据：data = {"ts":int,"voltage":double,"current":double,"percent":int,"temp":double}
    void BatteryData(const CTL::JsonObject& data) const;
    //电机状态
    void MotorStatus(const CTL::JsonObject& data) const;
    //加速度
    void Accelerometer(const CTL::JsonObject& data) const;
};

#endif //DRONE_TELEMETRYSERVICE_H
