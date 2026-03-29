#include "OAUser.h"

OAUser OAUser::ERROR_USER = OAUser();

OAUser::OAUser() {}

OAUser::OAUser(string username, string password, string department) {
    _username = username;
    _password = password;
    _department = department;
}

string OAUser::username() { return _username; }

string OAUser::department() { return _department; }

int OAUser::bookMeetingRoom(int mid) {
    return OADataCenter::getInstance().bookMeetingRoom(*this, mid);
}
