#include "../include/DataBase.h"


void DataBase_t::init() {
    db.set_type(CTL::DatabaseType::SQLite);
    const bool F = db.connect("./data.db");
    if (!F) {
        CTL::System::Println("DataBase_t connect Error");
        return;
    }
    persistent_.bind(&db);
    // 每种实体：建表 + 把已有数据载入各自缓存
    InitTable<User>();
    InitTable<GPSData>();
    InitTable<IMUData>();
    InitTable<BatteryData>();
    InitTable<BaroData>();
}
