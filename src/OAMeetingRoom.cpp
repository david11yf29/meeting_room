#include "OAMeetingRoom.h"
#include "OAUser.h"

OAMeetingRoom::OAMeetingRoom() {
    _mid = 0;
    _capacity = 0;
}

OAMeetingRoom::OAMeetingRoom(int mid, int capacity) {
    _mid = mid;
    _capacity = capacity;
}

int OAMeetingRoom::mid() { return _mid; }

int OAMeetingRoom::capacity() { return _capacity; }

OAUser& OAMeetingRoom::user() { return _user; }
