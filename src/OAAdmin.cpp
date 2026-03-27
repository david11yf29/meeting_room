#include "OAAdmin.h"

OAAdmin::OAAdmin() {}

OAAdmin::OAAdmin(string username, string password) {
    _username = username;
    _password = password;
}

string OAAdmin::username() { return _username; }

OAAdmin OAAdmin::ERROR_ADMIN = OAAdmin();