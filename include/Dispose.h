#ifndef DRONE_DISPOSE_H
#define DRONE_DISPOSE_H

#include "CCSystem.h"
#include "FrameRoute.h"

using Data_ = std::function<void(CTL::ByteArray&)>;

class Dispose {
public:
    bool InitBind(const Data_ UserVerify, const Data_ GPSLocation, const Data_ IMUData,const Data_ BaroData
        , const Data_ BatteryData, const Data_ MotorStatus, const Data_ Accelerometer);
    //用户验证函数
    int UserVerify(CTL::ByteArray &data);
    //GPS定位函数
    void GPSLocation(CTL::ByteArray &data);
    //IMU数据函数
    void IMUData(CTL::ByteArray &data);
    //气压计数据函数
    void BaroData(CTL::ByteArray &data);
    //电池数据函数
    void BatteryData(CTL::ByteArray &data);
    //电机状态函数
    void MotorStatus(CTL::ByteArray &data);
    //加速度函数
    void Accelerometer(CTL::ByteArray &data);
private:
    FrameParser parser_;
    CCMap<std::string , Data_> DisposeMap_;
};

#endif
