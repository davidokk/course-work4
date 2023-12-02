#pragma once

string randomString(int n) {
    string res;
    while (n--) {
        res += 'a' + rand() % 26;
    }
    return res;
}