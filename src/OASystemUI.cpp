#include "OASystemUI.h"
#include <iostream>
#include "OAUtils.h"
using namespace std;

// 顯示登錄介面
void showLogin(int flag) {}

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
                cout << "Admin Login" << endl;
                break;
            case 2:
                cout << "User Login" << endl;
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