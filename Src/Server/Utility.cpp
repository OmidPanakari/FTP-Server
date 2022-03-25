#include "Utility.hpp"
#include <sstream>

using namespace std;

vector<string> Utility::Split(string str, char c) {
    vector<string> result;
    string temp;
    for (uint i = 0; i < str.size(); i++) {
        if (str[i] != c) temp += str[i];
        else{
            result.push_back(temp);
            temp = "";
        }
    }
    if (!temp.empty()) result.push_back(temp);
    return result;
}

string Utility::ToStr(int n) {
    stringstream ss;
    ss << n;
    return ss.str();
}
