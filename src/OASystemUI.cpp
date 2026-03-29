#include "OASystemUI.h"
#include <iostream>
#include "OAUtils.h"
#include "OADataCenter.h"

using namespace std;

#define FLAG_ADMIN 1
#define FLAG_USER 0

void showAdminAddUserPage(OAAdmin& admin) {
    system("clear");
    string username, password, department;
    cout << "Please input username for add user: ";
    cin >> username;
    cout << "Please input password for this user: ";
    cin >> password;
    cout << "Please input department for add user: ";
    cin >> department;

    if (admin.addUser(username, password, department)) {
        cout << "Add user successfully" << endl;
    } else {
        cout << "Add user failed, this user already existed" << endl;
    }
    cout << "Press Enter to continue...";
    cin.get();
}

void showAdminDeleteUserPage(OAAdmin& admin) {
    system("clear");
    string username;
    cout << "Please input user you want to delete: ";
    cin >> username;

    if (admin.deleteUser(username)) {
        cout << "Delete user successfully" << endl;
    } else {
        cout << "Delete user fail, there is no such user" << endl;
    }
}

void showAdminListUsers(OAAdmin& admin) { admin.showAllUsers(); }

void showAdminAddMeetingRoomPage(OAAdmin& admin) {
    system("clear");
    int mid, capacity;
    cout << "Please input the No. of meeting room: ";
    mid = OAUtils::inputNumber();
    cout << "Please input the capacity of meeting room: ";
    capacity = OAUtils::inputNumber();

    if (admin.addMeetingRoom(mid, capacity)) {
        cout << "Add meeting room success! " << endl;
    } else {
        cout << "Add meeting room fail. This meeting room already existed "
             << endl;
    }
}

void showAdminDeleteMeetingRoomPage(OAAdmin& admin) {
    system("clear");
    int mid;
    cout << "Please input the No. of meeting room you want to delete: ";
    mid = OAUtils::inputNumber();

    if (admin.deleteMeetingRoom(mid)) {
        cout << "Delete meeting room success! " << endl;
    } else {
        cout << "Delete meeting room fail. This meeting room is not existed"
             << endl;
    }
}

void showAdminMainPage(OAAdmin& admin) {
    while (1) {
        system("clear");
        cout << "Login success, welcome back bro!" << admin.username() << endl;
        cout << "----------------------------------------" << endl;
        cout << "1. Add an user" << endl;
        cout << "2. Delete an user" << endl;
        cout << "3. List all user" << endl;
        cout << "4. Add a meeting room" << endl;
        cout << "5. Delete a meeting room" << endl;
        cout << "6. List all meeting room" << endl;
        cout << "7. Reset all meeting room status" << endl;
        cout << "8. Exit admin login" << endl;
        cout << "----------------------------------------" << endl;
        cout << "Please input your number: ";

        int c = OAUtils::inputNumber();

        switch (c) {
            case 1:
                showAdminAddUserPage(admin);
                break;
            case 2:
                showAdminDeleteUserPage(admin);
                break;
            case 3:
                showAdminListUsers(admin);
                break;
            case 4:
                showAdminAddMeetingRoomPage(admin);
                break;
            case 5:
                showAdminDeleteMeetingRoomPage(admin);
                break;
            case 6:
                system("clear");
                admin.showAllMeetingRooms();
                break;
            case 7:
                admin.clearMeetingRoomStatus();
                break;
            case 8:
                goto end;
        }
        cout << "Press Enter to continue...";
        cin.get();
    }
end:
    cout << "Bye, see you next time!" << endl;
}

void showUserBookMeetingRoomPage(OAUser& user) {
    system("clear");
    cout << "Please input the No. of meeting room you want to reserve: "
         << endl;
    int mid = OAUtils::inputNumber();

    int res = user.bookMeetingRoom(mid);
    if (res == MEETING_ROOM_BOOK_SUCCESS) {
        cout << "Meeting room booked successfully! " << endl;
    } else if (res == MEETING_ROOM_BOOKED) {
        cout << "Booked fail, this meeting room already be booked! " << endl;
    } else if (res == MEETING_ROOM_NOT_FOUND) {
        cout << "Booked fail, this meeting room do not exist! " << endl;
    }
}

void showUserCancelBookMeetingRoom(OAUser& user) {
    system("clear");
    cout << "Please input the No. of meeting room you want to cancel: " << endl;
    int mid = OAUtils::inputNumber();

    int res = user.cancelBookMeetingRoom(mid);
    if (res == CANCEL_MEETING_ROOM_SUCCESS) {
        cout << "Cancel booked meeting room successfully! " << endl;
    } else if (res == CANCEL_MEETING_ROOM_FAIL) {
        cout << "Cancel fail, you do not book this meeting room! " << endl;
    } else if (res == CANCEL_MEETING_ROOM_NOT_FOUND) {
        cout << "Cancel fail, this meeting room do not exist! " << endl;
    }
}

void showUserMainPage(OAUser& user) {
    while (1) {
        system("clear");
        cout << "Login success, welcome " << user.department() << "'s "
             << user.username() << endl;
        cout << "----------------------------------------" << endl;
        cout << "1. Reserve a meeting room" << endl;
        cout << "2. Cancel a reserved meeting room" << endl;
        cout << "3. List all reserved meeting room" << endl;
        cout << "4. Exit user login" << endl;
        cout << "----------------------------------------" << endl;
        cout << "Please input your operation: " << endl;

        int c = OAUtils::inputNumber();

        switch (c) {
            case 1:
                showUserBookMeetingRoomPage(user);
                break;
            case 2:
                showUserCancelBookMeetingRoom(user);
                break;
            case 3:
                system("clear");
                user.showAllBookedMeetingRooms();
                break;
            case 4:
                goto end;
        }
        cout << "Press Enter to continue...";
        cin.get();
    }
end:
    cout << "Bye, see you next time!" << endl;
}

// 顯示登錄介面, flag 表示身分
// 1: 管理員登錄
// 0: 普通用戶登錄
void showLogin(int flag) {
    cout << "\033[2J\033[H";
    if (flag == FLAG_ADMIN) {
        cout << "Admin Login" << endl;
    } else {
        cout << "User Login" << endl;
    }
    cout << "----------------------------------------" << endl;

    string username, password;
    cout << "Please input Username: ";
    cin >> username;
    cout << "Please input Password: ";
    cin >> password;

    OADataCenter& dataCenter = OADataCenter::getInstance();

    if (flag == FLAG_ADMIN) {
        OAAdmin& admin = dataCenter.loginAdmin(username, password);
        if (&admin == &(OAAdmin::ERROR_ADMIN)) {
            cout << "Login failed, return to previous level" << endl;
        } else {
            showAdminMainPage(admin);
        }
    } else {
        OAUser& user = dataCenter.loginUser(username, password);
        if (&user == &(OAUser::ERROR_USER)) {
            cout << "Login failed, return to previous level" << endl;
        } else {
            showUserMainPage(user);
        }
    }
}

void OASystemUI::displayMainPage() {
    while (1) {
        cout << "Welcome to the OA Meeting Reservation System" << endl;
        cout << "----------------------------------------" << endl;
        cout << "1. Admin Login" << endl;
        cout << "2. User Login" << endl;
        cout << "3. Quit" << endl;
        cout << "----------------------------------------" << endl;
        cout << "Please select a number: " << endl;

        // 讓用戶進行控制台的輸入選擇
        int choice = OAUtils::inputNumber();

        switch (choice) {
            case 1:
                showLogin(FLAG_ADMIN);
                break;
            case 2:
                showLogin(FLAG_USER);
                break;
            case 3:
                goto end;
        }
        cout << "Press Enter to continue...";
        cin.get();
        cout << "\033[2J\033[H";
    }
end:
    cout << "Thanks and see you next time!" << endl;
}