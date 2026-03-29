#pragma once
#include <iostream>
#include "OADataCenter.h"
using namespace std;

class OAUser {
    friend class OADataCenter;

   private:
    string _username;
    string _password;
    string _department;

   public:
    OAUser();
    OAUser(string username, string password, string department);
    static OAUser ERROR_USER;

    string username();
    string department();

    int bookMeetingRoom(int mid);
    int cancelBookMeetingRoom(int mid);
    void showAllBookedMeetingRooms();
};