#include "service/UserService.h"
#include "infra/DataBase.h"

//用户验证：校验账号密码与库中记录一致
void UserService::UserVerify(const CTL::JsonObject& data) const {
    if (!data.isObject() || !data.has("ID") || !data.has("account") || !data.has("password")) {
        CTL::System::Println("[UserService] UserVerify 参数缺失");
        return;
    }
    const int id = data.get("ID").asInt();
    const std::string account = data.get("account").asString();
    const std::string password = data.get("password").asString();

    User* user_t = User::Get(id);
    if (user_t == nullptr) {
        CTL::System::Println("[UserService] ID:{} , 用户不存在", id);
        return;
    }
    if (account != user_t->account || password != user_t->password) {
        CTL::System::Println("[UserService] ID:{} , 用户账号或密码错误", id);
        return;
    }
    CTL::System::Println("[UserService] ID:{} 验证通过", id);
    // TODO: 构造结果 JSON 通过传输层回传客户端（待发送通道接入）
}
