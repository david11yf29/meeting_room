#pragma once
#include <vector>
#include "OAAdmin.h"
#include "OAUser.h"
#include "OAMeetingRoom.h"

class OAAdmin;
class OAUser;
class OAMeetingRoom;

class OADataCenter {
   private:
    vector<OAAdmin> admins;
    vector<OAUser> users;
    vector<OAMeetingRoom> rooms;

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

    // 添加會議室, 返回值是否添加成功
    bool addMeetingRoom(int mid, int capacity);
    // 刪除會議室
    bool deleteMeetingRoom(int mid);

    vector<OAUser> getUsers();
    vector<OAMeetingRoom> getRooms();
};