#pragma once
#include <iostream>
using namespace std;

class OAUser {
   private:
    string _username;
    string _password;
    string _department;

   public:
    OAUser();
    OAUser(string username, string password, string department);
};