#include "OADataCenter.h"

OADataCenter OADataCenter::_instance = OADataCenter();

OADataCenter::OADataCenter() {
    admins.push_back(OAAdmin("admin", "admin"));
    users.push_back(OAUser("zhangsan", "123123", "Admin Resource"));
    users.push_back(OAUser("lisi", "123456", "HR Resource"));
}

OADataCenter& OADataCenter::getInstance() { return _instance; }

OAAdmin& OADataCenter::loginAdmin(string username, string password) {
    for (OAAdmin& admin : admins) {
        if (admin._username == username && admin._password == password) {
            return admin;
        }
    }
    return OAAdmin::ERROR_ADMIN;
}

OAUser& OADataCenter::loginUser(string username, string password) {
    for (OAUser& user : users) {
        if (user._username == username && user._password == password) {
            return user;
        }
    }
    return OAUser::ERROR_USER;
}

bool OADataCenter::addUser(string username, string password,
                           string department) {
    // 檢查用戶是否存在
    for (OAUser& user : users) {
        if (user._username == username) {
            return false;
        }
    }
    // 添加用戶
    users.push_back(OAUser(username, password, department));
    return true;
}

bool OADataCenter::deleteUser(string username) {
    auto it = users.begin();
    for (; it != users.end(); it++) {
        if (it->username() == username) {
            break;
        }
    }

    if (it == users.end()) {
        return false;
    }

    users.erase(it);
    return true;
}
