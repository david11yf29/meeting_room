#include "OASystemUI.h"
#include <iostream>
#include "OAUtils.h"
using namespace std;

#define FLAG_ADMIN 1
#define FLAG_USER 0

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
}

void OASystemUI::displayMainPage() {
    while (1) {
        cout << "Welcome to the OA Meeting Reservation System" << endl;
        cout << "----------------------------------------" << endl;
        cout << "1. Admin Login" << endl;
        cout << "2. User Login" << endl;
        cout << "3. Quit" << endl;
        cout << "----------------------------------------" << endl;
        cout << "Please Select a number: " << endl;

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