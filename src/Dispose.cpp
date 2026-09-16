#include "../include//Dispose.h"
#include "../include/DataBase.h"
#include <filesystem>

bool Dispose::InitBind(const Data_ UserVerify, const Data_ GPSLocation, const Data_ IMUData, const Data_ BaroData,
    const Data_ BatteryData, const Data_ MotorStatus, const Data_ Accelerometer) {

    DisposeMap_.insert(std::pair("UserVerify", UserVerify));
    DisposeMap_.insert(std::pair("GPSLocation", GPSLocation));
    DisposeMap_.insert(std::pair("IMUData", IMUData));
    DisposeMap_.insert(std::pair("BaroData", BaroData));
    DisposeMap_.insert(std::pair("BatteryData", BatteryData));
    DisposeMap_.insert(std::pair("MotorStatus", MotorStatus));
    DisposeMap_.insert(std::pair("Accelerometer", Accelerometer));
    return true;
}

int Dispose::UserVerify(CTL::ByteArray &data) {
    User user;//用户
    //数据库
    int ret = data.indexOf("ID");
    if (ret < 0) {
        return -1;
    }
    std::memcpy(&user.ID, data.buffer() + ret, sizeof(user));
    // 查询已连接的 User.db（表已存在则 createTable 只做结构同步，不重建、不清数据）
    User *user_t = User::Get(user.ID);
    if (user_t == nullptr) {
        CTL::System::Println("ID:{} , 用户不存在", user.ID);
        return -1;
    }
    if (user.account != user_t->account || user.password != user_t->password) {
        //send
        CTL::System::Println("ID:{} , 用户账号或密码错误", user.ID);
        return -1;
    }
    return 0;
}

void Dispose::GPSLocation(CTL::ByteArray &data) {
}

void Dispose::IMUData(CTL::ByteArray &data) {
}

void Dispose::BaroData(CTL::ByteArray &data) {
}

void Dispose::BatteryData(CTL::ByteArray &data) {
}

void Dispose::MotorStatus(CTL::ByteArray &data) {
}

void Dispose::Accelerometer(CTL::ByteArray &data) {
}

