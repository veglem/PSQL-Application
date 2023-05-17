//
// Created by vzhur on 16.05.2023.
//

#ifndef BOOSTBEASTSERVER_URLDECODE_H
#define BOOSTBEASTSERVER_URLDECODE_H

#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>

using namespace std;
//
//string url_encode(const string &value) {
//    ostringstream escaped;
//    escaped.fill('0');
//    escaped << hex;
//
//    for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
//        string::value_type c = (*i);
//
//        // Keep alphanumeric and other accepted characters intact
//        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
//            escaped << c;
//            continue;
//        }
//
//        // Any other characters are percent-encoded
//        escaped << uppercase;
//        escaped << '%' << setw(2) << int((unsigned char) c);
//        escaped << nouppercase;
//    }
//
//    return escaped.str();
//}

string url_Decode(string &SRC);

#endif //BOOSTBEASTSERVER_URLDECODE_H
