#ifndef DRONE_USERSERVICE_H
#define DRONE_USERSERVICE_H

#include "CCSystem.h"
#include "JSON/Json.hpp"

//业务层：用户鉴权服务（FT_CONTROL 大类）
class UserService {
public:
    //用户验证：data = {"ID":int,"account":str,"password":str}
    void UserVerify(const CTL::JsonObject& data) const;
};

#endif //DRONE_USERSERVICE_H
