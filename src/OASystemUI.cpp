#include "OASystemUI.h"
#include <iostream>
using namespace std;

void OASystemUI::displayMainPage() {
    while (1) {
        cout << "Welcome to the OA Meeting Reservation System" << endl;
        cout << "----------------------------------------" << endl;
        cout << "1. Admin Login" << endl;
        cout << "2. User Login" << endl;
        cout << "3. Quit" << endl;
        cout << "Please Select a number" << endl;
    }
}