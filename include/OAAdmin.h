#pragma once
#include <iostream>
using namespace std;

class OAAdmin {
   private:
    string _username;
    string _password;

   public:
    OAAdmin();
    OAAdmin(string username, string password);
};