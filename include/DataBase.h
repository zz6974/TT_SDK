#ifndef DRONE_DATABASE_H
#define DRONE_DATABASE_H

#include "sqlite.h"


/// 注册登录用户表
class User : public EntityBase<User> {
    PERSISTENT_INTRUSIVE(User, account, password, ID)
public:
    int ID = 0;
    std::string account;
    std::string password;
};

/// GPS 定位数据表
class GPSData : public EntityBase<GPSData> {
    PERSISTENT_INTRUSIVE(GPSData, ID, ts, lat, lng, alt, fix, sat)
public:
    int ID = 0;
    int ts = 0;         ///< Unix 秒（Persistent::insert 的 INTEGER 仅支持 int）
    double lat = 0.0;   ///< 纬度
    double lng = 0.0;   ///< 经度
    double alt = 0.0;   ///< 海拔
    int fix = 0;        ///< 定位状态
    int sat = 0;        ///< 卫星数
};

/// IMU 惯性测量单元数据表
class IMUData : public EntityBase<IMUData> {
    PERSISTENT_INTRUSIVE(IMUData, ID, ts, ax, ay, az, gx, gy, gz)
public:
    int ID = 0;
    int ts = 0;         ///< Unix 秒
    double ax = 0.0, ay = 0.0, az = 0.0;   ///< 三轴加速度
    double gx = 0.0, gy = 0.0, gz = 0.0;   ///< 三轴角速度
};

/// 电池状态数据表
class BatteryData : public EntityBase<BatteryData> {
    PERSISTENT_INTRUSIVE(BatteryData, ID, ts, voltage, current, percent, temp)
public:
    int ID = 0;
    int ts = 0;
    double voltage = 0.0;   ///< 电压 V
    double current = 0.0;   ///< 电流 A
    int percent = 0;        ///< 剩余电量 %
    double temp = 0.0;      ///< 电池温度
};

/// 气压计数据表
class BaroData : public EntityBase<BaroData> {
    PERSISTENT_INTRUSIVE(BaroData, ID, ts, pressure, altitude, temp)
public:
    int ID = 0;
    int ts = 0;
    double pressure = 0.0;  ///< 气压
    double altitude = 0.0;  ///< 推算高度
    double temp = 0.0;
};
class DataBase_t {
    inline static CTL::Persistent persistent_;
    inline static CTL::database db;
public:
    static void init();

    /// 通用建表 + 缓存装载（新实体加一行 init 调用即可）
    template<typename T>
    static void InitTable() {
        const auto ret = persistent_.createTable<T>({}, {"ID"});
        if (!ret.Flag) {
            CTL::System::Println("DataBase_t Init CreateTable Error");
            return;
        }
        const auto list = persistent_.query<T>("");
        CTL::Map<int, T> items;
        for (const auto& item : list) {
            items.put(item.ID, item);
        }
        T::Init(items);
    }
    static CTL::Persistent* getPersistent() {
        return &persistent_;
    }
};

template<typename Derived>
int EntityBase<Derived>::GetID() {
    CCUniqueLock lock(mutex_t);
    for (int i = 0; i < 100000000; i++) {
        if (!map_.IsContains(i)) {
            return i;
        }
    }
    return -1;
}

template<typename Derived>
void EntityBase<Derived>::Init(const CTL::Map<int, Derived>& items) {
    CCUniqueLock lock(mutex_t);
    map_ = items;
}

template<typename Derived>
void EntityBase<Derived>::Add(const Derived& item) {
    const auto ret = DataBase_t::getPersistent()->insert(item);
    if (ret.Flag) {
        CCUniqueLock lock(mutex_t);
        map_.put(item.ID, item);
    }
}

template<typename Derived>
void EntityBase<Derived>::Update(const Derived& item) {
    const auto ret = DataBase_t::getPersistent()->update(item, "ID = {}", item.ID);
    if (ret.Flag) {
        CCUniqueLock lock(mutex_t);
        map_.put(item.ID, item);
    }
}

template<typename Derived>
void EntityBase<Derived>::Remove(int ID) {
    const auto ret = DataBase_t::getPersistent()->remove<Derived>("ID = {}", ID);
    if (ret.Flag) {
        CCUniqueLock lock(mutex_t);
        map_.remove(ID);
    }
}

#endif //DRONE_DATABASE_H
