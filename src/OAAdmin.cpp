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

OAAdmin OAAdmin::ERROR_ADMIN = OAAdmin();