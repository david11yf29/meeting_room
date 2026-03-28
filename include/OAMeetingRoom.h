#pragma once
#include "OAUser.h"
#include "OADataCenter.h"

class OAMeetingRoom {
    friend class OADataCenter;

   private:
    int _mid;
    int _capacity;
    OAUser& _user = OAUser::ERROR_USER;

   public:
    OAMeetingRoom();
    OAMeetingRoom(int mid, int capacity);

    int mid();
    int capacity();
    OAUser& user();
};