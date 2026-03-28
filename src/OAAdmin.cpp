#include "OAAdmin.h"
#include "OADataCenter.h"

OAAdmin::OAAdmin() {}

OAAdmin::OAAdmin(string username, string password) {
    _username = username;
    _password = password;
}

string OAAdmin::username() { return _username; }

bool OAAdmin::addUser(string username, string password, string department) {
    return OADataCenter::getInstance().addUser(username, password, department);
}

bool OAAdmin::deleteUser(string username) {
    return OADataCenter::getInstance().deleteUser(username);
}

void OAAdmin::showAllUsers() {
    vector<OAUser> users = OADataCenter::getInstance().getUsers();
    system("clear");
    for (OAUser& u : users) {
        cout << "User: " << u.username() << ", Department: " << u.department()
             << endl;
    }
}

bool OAAdmin::addMeetingRoom(int mid, int capacity) {
    return OADataCenter::getInstance().addMeetingRoom(mid, capacity);
}

bool OAAdmin::deleteMeetingRoom(int mid) {
    return OADataCenter::getInstance().deleteMeetingRoom(mid);
}

void OAAdmin::clearMeetingRoomStatus() {
    OADataCenter::getInstance().clearMeetingRoomStatus();
}

void OAAdmin::showAllMeetingRooms() {
    vector<OAMeetingRoom> rooms = OADataCenter::getInstance().getRooms();
    for (OAMeetingRoom& m : rooms) {
        cout << "MeetingRoom No.: " << m.mid()
             << ", capacity: " << m.capacity();
        if (m.user() == &(OAUser::ERROR_USER)) {
            cout << ", reservable! " << endl;
        } else {
            cout << ", this meeting room is reserved by "
                 << m.user()->department() << "'s " << m.user()->username() << "!"
                 << endl;
        }
    }
}

OAAdmin OAAdmin::ERROR_ADMIN = OAAdmin();