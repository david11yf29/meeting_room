#include "OAMeetingRoom.h"
#include "OAUser.h"

OAMeetingRoom::OAMeetingRoom() {
    _mid = 0;
    _capacity = 0;
    _user = &OAUser::ERROR_USER;
}

OAMeetingRoom::OAMeetingRoom(int mid, int capacity) {
    _mid = mid;
    _capacity = capacity;
    _user = &OAUser::ERROR_USER;
}

int OAMeetingRoom::mid() { return _mid; }

int OAMeetingRoom::capacity() { return _capacity; }

OAUser* OAMeetingRoom::user() { return _user; }
