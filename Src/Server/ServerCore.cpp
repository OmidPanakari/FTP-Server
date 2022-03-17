#include "ServerCore.hpp"
#include <sys/stat.h>

using namespace std;

ServerCore::ServerCore(JsonSerializer config)
{
    ReadUsers(config);
    ReadAdminFiles(config);
    maxAllowedConnections = config.GetInteger("maxAllowedConnections");
    struct stat buffer;
    if (stat(ROOT, &buffer) != 0)
    {
        mkdir(ROOT, 0777);
    }
}

void ServerCore::ReadUsers(JsonSerializer config)
{
    users.clear();
    for (auto userConfig : config.GetArray("users"))
    {
        User newUser;
        newUser.username = userConfig.Get("username");
        newUser.password = userConfig.Get("password");
        newUser.isAdmin = userConfig.Get("isAdmin") == "True" ? true : false;
        users.push_back(newUser);        
    }
}

void ServerCore::ReadAdminFiles(JsonSerializer config)
{
    for (auto file : config.GetStrArray("privilegedFiles")){
        adminFiles.insert(file);
    };
}

int ServerCore::CheckUsername(string username, int clientID) {
    if (loggedInUsers.size() > maxAllowedConnections)
        return 501;
    for (int i = 0; i < users.size(); i++) {
        if (users[i].username == username)
        {
            OnlineUser newLogin;
            newLogin.user = &users[i];
            newLogin.isAuthenticated = false;
            loggedInUsers[clientID] = newLogin;
            return 331;
        }
    }
    return 500;
}

bool ServerCore::IsAuthenticated(int clientID) {
    auto user = loggedInUsers.find(clientID);
    if(user != loggedInUsers.end()){
        if (loggedInUsers[clientID].isAuthenticated){
            return true;
        }
    }
    return false;
}

int ServerCore::Authenticate(string password, int clientID) {
    auto user = loggedInUsers.find(clientID);
    if(user != loggedInUsers.end()){
        OnlineUser* onlineUser = &user->second;
        if(onlineUser->user->password == password){
            onlineUser->isAuthenticated = true;
            onlineUser->directory.clear();
            cout << loggedInUsers[clientID].isAuthenticated << endl;
            return 230;
        }
        return 500;
    }
    return 503;
}

vector <string> ServerCore::Split(string str, char c) {
    vector<string> result;
    string temp;
    for (int i = 0; i < str.size(); i++) {
        if (str[i] != c) temp += str[i];
        else{
            result.push_back(temp);
            temp = "";
        }
    }
    if (!temp.empty()) result.push_back(temp);
    return result;
}

string ServerCore::MakeDirStr(vector<string> pathStk)
{
    string dir = "";
    for (string p : pathStk)
    {
        dir += "/" + p;
    }
    if (dir == "")
        dir = "/";
    return ROOT + dir;
}

vector<string> ServerCore::ConvertDirectory(vector<string> current, string path) {
    vector<string> pathWay = Split(path, '/');
    if (path[0] == '/')
        current.clear();
    for (string p : pathWay)
    {
        if (p == "..")
        {
            if (current.size())
                current.pop_back();
        }
        else if (p != ".")
        {
            current.push_back(p);
        }
    }
    return current;
}

bool ServerCore::PathExists(vector<string> path)
{
    string dir = MakeDirStr(path);
    struct stat buffer;
    return (stat(dir.c_str(), &buffer) == 0);
}

GetDirectoryResponse ServerCore::GetCurrentDirectory(int clientID) {
    if (!IsAuthenticated(clientID)) return {332, ""};
    string dir = MakeDirStr(loggedInUsers[clientID].directory);
    return {257, dir};
}

int ServerCore::MakeDirectory(string path, int clientID)
{
    if (!IsAuthenticated(clientID)) return 323;
    auto dest = ConvertDirectory(loggedInUsers[clientID].directory, path);
    if (dest.empty())
        return 500;
    string newDir = dest.back();
    if (PathExists(dest))
        return 500;
    dest.pop_back();
    if (!PathExists(dest))
        return 500;
    dest.push_back(newDir);
    if (mkdir(MakeDirStr(dest).c_str(), 0777) == 0)
        return 257;
    return 500;
}






