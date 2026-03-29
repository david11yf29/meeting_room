#include "OADataCenter.h"
#include "OAUtils.h"

OADataCenter OADataCenter::_instance = OADataCenter();

OADataCenter::OADataCenter() {
    admins.push_back(OAAdmin("admin", "admin"));

    users.push_back(OAUser("zhangsan", "123123", "Admin Resource"));
    users.push_back(OAUser("lisi", "123456", "HR Resource"));

    rooms.push_back(OAMeetingRoom(1, 10));
    rooms.push_back(OAMeetingRoom(2, 6));
    rooms.push_back(OAMeetingRoom(3, 4));

    OAUser* user = new OAUser("xiaoxiao", "123123", "abc");
    OAMeetingRoom r(4, 12);
    r._user = user;
    rooms.push_back(r);
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

OAUser& OADataCenter::loginUser(string username, string password) {
    for (OAUser& user : users) {
        if (user._username == username && user._password == password) {
            return user;
        }
    }
    return OAUser::ERROR_USER;
}

bool OADataCenter::addUser(string username, string password,
                           string department) {
    // 檢查用戶是否存在
    for (OAUser& user : users) {
        if (user._username == username) {
            return false;
        }
    }
    // 添加用戶
    users.push_back(OAUser(username, password, department));
    return true;
}

bool OADataCenter::deleteUser(string username) {
    auto it = users.begin();
    for (; it != users.end(); it++) {
        if (it->username() == username) {
            break;
        }
    }

    if (it == users.end()) {
        return false;
    }

    users.erase(it);
    return true;
}

bool OADataCenter::addMeetingRoom(int mid, int capacity) {
    // 檢查會議室是否存在
    for (OAMeetingRoom& room : rooms) {
        if (room._mid == mid) {
            return false;
        }
    }
    // 添加會議室
    rooms.push_back(OAMeetingRoom(mid, capacity));
    return true;
}

bool OADataCenter::deleteMeetingRoom(int mid) {
    vector<OAMeetingRoom>::iterator it = rooms.begin();
    for (; it != rooms.end(); it++) {
        if (it->_mid == mid) {
            break;
        }
    }

    if (it == rooms.end()) {
        return false;
    }

    rooms.erase(it);
    return true;
}

void OADataCenter::clearMeetingRoomStatus() {
    for (OAMeetingRoom& room : rooms) {
        room._user = &OAUser::ERROR_USER;
    }
}

vector<OAUser> OADataCenter::getUsers() {
    vector<OAUser> users_back(users);
    return users_back;
}

vector<OAMeetingRoom> OADataCenter::getRooms() {
    return vector<OAMeetingRoom>(rooms);
}

int OADataCenter::bookMeetingRoom(OAUser& user, int mid) {
    auto it = rooms.begin();
    for (; it != rooms.end(); it++) {
        if (it->_mid == mid) {
            break;
        }
    }

    if (it == rooms.end()) {
        return MEETING_ROOM_NOT_FOUND;
    }

    if (it->_user != &OAUser::ERROR_USER) {
        return MEETING_ROOM_BOOKED;
    }

    it->_user = &user;
    return MEETING_ROOM_BOOK_SUCCESS;
}

int OADataCenter::cancelBookMeetingRoom(OAUser& user, int mid) {
    auto it = rooms.begin();
    for (; it != rooms.end(); it++) {
        if (it->_mid == mid) {
            break;
        }
    }
    if (it == rooms.end()) {
        return CANCEL_MEETING_ROOM_NOT_FOUND;
    }

    if (it->_user != &user) {
        return CANCEL_MEETING_ROOM_FAIL;
    }

    it->_user = &OAUser::ERROR_USER;

    return CANCEL_MEETING_ROOM_SUCCESS;
}
