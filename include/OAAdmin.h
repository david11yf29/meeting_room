#pragma once
#include <iostream>
#include "OADataCenter.h"
using namespace std;

class OAAdmin {
    friend class OADataCenter;

   private:
    string _username;
    string _password;

   public:
    OAAdmin();
    OAAdmin(string username, string password);
    static OAAdmin ERROR_ADMIN;

    string username();

    bool addUser(string username, string password, string department);
    bool deleteUser(string username);

    void showAllUsers();
};