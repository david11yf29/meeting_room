#include "OAUtils.h"
#include <iostream>
using namespace std;

int OAUtils::inputNumber() {
    int choice = -1;
    cin >> choice;
    while (cin.fail()) {
        // 緩衝區清空、錯誤旗標重置
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin >> choice;
    }

    return choice;
};