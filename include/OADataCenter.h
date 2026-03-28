#pragma once
#include <vector>
#include "OAAdmin.h"
#include "OAUser.h"

class OAAdmin;
class OAUser;

class OADataCenter {
   private:
    vector<OAAdmin> admins;
    vector<OAUser> users;
    OADataCenter();
    static OADataCenter _instance;

   public:
    static OADataCenter& getInstance();
    OAAdmin& loginAdmin(string username, string password);
    OAUser& loginUser(string username, string password);

    // 添加用戶, 返回值是否添加成功
    bool addUser(string username, string passoword, string department);
    // 刪除用戶
    bool deleteUser(string username);

    vector<OAUser> getUsers();
};