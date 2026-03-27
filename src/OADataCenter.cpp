#include "OADataCenter.h"

OADataCenter OADataCenter::_instance = OADataCenter();

OADataCenter::OADataCenter() {
    admins.push_back(OAAdmin("admin", "admin"));
    users.push_back(OAUser("zhangsan", "123123", "行政部"));
    users.push_back(OAUser("lisi", "123456", "人力資源部"));
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
