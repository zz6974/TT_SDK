#include "service/TelemetryService.h"
#include "infra/DataBase.h"

//GPS定位：解析后入库（遥测 handler 的参考实现）
void TelemetryService::GPSLocation(const CTL::JsonObject& data) const {
    if (!data.isObject() || !data.has("ts") || !data.has("lat") || !data.has("lng")) {
        CTL::System::Println("[Telemetry] GPSLocation 参数缺失");
        return;
    }
    GPSData gps;
    gps.ID  = GPSData::GetID();
    gps.ts  = data.get("ts").asInt();
    gps.lat = data.get("lat").asDouble();
    gps.lng = data.get("lng").asDouble();
    if (data.has("alt")) gps.alt = data.get("alt").asDouble();
    if (data.has("fix")) gps.fix = data.get("fix").asInt();
    if (data.has("sat")) gps.sat = data.get("sat").asInt();
    GPSData::Add(gps);
    CTL::System::Println("[Telemetry] GPS 入库 ID:{} lat:{} lng:{}", gps.ID, gps.lat, gps.lng);
}

//IMU数据：解析三轴加速度与角速度后写入 IMUData 实体
void TelemetryService::IMUData(const CTL::JsonObject& data) const {
    // TODO: 参照 GPSLocation 实现：解析 ts/ax/ay/az/gx/gy/gz 后 IMUData::Add 入库
    CTL::System::Println("[Telemetry] IMUData 待实现");
}

//气压计数据
void TelemetryService::BaroData(const CTL::JsonObject& data) const {
    // TODO: 解析 ts/pressure/altitude/temp 后 BaroData::Add 入库
    CTL::System::Println("[Telemetry] BaroData 待实现");
}

//电池数据
void TelemetryService::BatteryData(const CTL::JsonObject& data) const {
    // TODO: 解析 ts/voltage/current/percent/temp 后 BatteryData::Add 入库
    CTL::System::Println("[Telemetry] BatteryData 待实现");
}

//电机状态
void TelemetryService::MotorStatus(const CTL::JsonObject& data) const {
    // TODO: 尚无 MotorStatus 实体表，需在 infra/DataBase.h 新增实体后入库
    CTL::System::Println("[Telemetry] MotorStatus 待实现");
}

//加速度
void TelemetryService::Accelerometer(const CTL::JsonObject& data) const {
    // TODO: 尚无 Accelerometer 实体表，需在 infra/DataBase.h 新增实体后入库
    CTL::System::Println("[Telemetry] Accelerometer 待实现");
}
