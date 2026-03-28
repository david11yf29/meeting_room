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

OAAdmin OAAdmin::ERROR_ADMIN = OAAdmin();