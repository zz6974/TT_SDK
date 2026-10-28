#include "infra/DataBase.h"


void DataBase_t::init() {
    db.set_type(CTL::DatabaseType::SQLite);
    // 使用工程根目录绝对路径拼接数据库文件，避免运行目录不同导致路径错位
    const std::string dbPath = std::string(PROJECT_ROOT_DIR) + "/data.db";
    const bool F = db.connect(dbPath);
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
