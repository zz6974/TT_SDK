#ifndef DRONE_SQLITE_H
#define DRONE_SQLITE_H

#include "CCSQLite3.h"
#include "CCSystem.h"
#include "Persistent.h"
#include "TL/Map.h"
#include "CCAutoLock.h"
#include "CCVector.h"

template<typename Derived>
class EntityBase {
protected:
    inline static CTL::Map<int, Derived> map_;  ///< 内存缓存（线程安全）
    inline static std::mutex mutex_t;           ///< 缓存锁
public:
    static int GetID();                                     ///< 分配一个未使用的 ID
    static void Init(const CTL::Map<int, Derived>& items);  ///< 数据库数据载入缓存
    static void Add(const Derived& item);                   ///< 入库 + 更新缓存
    static void Update(const Derived& item);                ///< 更新数据库 + 缓存
    static void Remove(int ID);                             ///< 删除数据库记录 + 缓存
    static Derived* Get(int ID) {
        CCUniqueLock lock(mutex_t);
        if (!map_.IsContains(ID)) {
            return nullptr;
        }
        return map_.Get(ID);
    }
    static CCVector<Derived> GetAll() {
        CCUniqueLock lock(mutex_t);
        return map_.Values();
    }
};

#endif //DRONE_SQLITE_H
