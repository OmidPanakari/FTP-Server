#include "ServerCore.hpp"
#include <sys/stat.h>
#include <filesystem>

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
    return 430;
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
        return 430;
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

string ServerCore::MakeDirStr(vector<string> pathStk, bool addRoot = true)
{
    string dir = "";
    for (string p : pathStk)
    {
        dir += "/" + p;
    }
    if (dir == "")
        dir = "/";
    return addRoot ? ROOT + dir : dir;
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

int ServerCore::RenameFile(std::string path, std::string newPath, int clientID) {
    if (!IsAuthenticated(clientID)) return 332;
    auto currentDirectory = loggedInUsers[clientID].directory;
    auto dest = ConvertDirectory(currentDirectory, path);
    auto old = dest.back();
    if (!PathExists(dest))
        return 500;
    dest.pop_back();
    auto destStr = MakeDirStr(dest) + "/";
    if (!loggedInUsers[clientID].user->isAdmin && adminFiles.find(destStr + old) != adminFiles.end())
        return 550;
    rename((destStr + old).c_str(), (destStr + newPath).c_str());
    return 250;
}

int ServerCore::DeleteFileOrDirectory(std::string path, int clientID) {
    if (!IsAuthenticated(clientID)) return 332;
    auto currentDirectory = loggedInUsers[clientID].directory;
    auto dest = ConvertDirectory(currentDirectory, path);
    auto destStr = MakeDirStr(dest);
    if (!PathExists(dest))
        return 500;
    if (!loggedInUsers[clientID].user->isAdmin && adminFiles.find(destStr) != adminFiles.end())
        return 550;
    remove(destStr.c_str());
    return 250;
}

int ServerCore::ChangeDirectory(std::string path, int clientID) {
    if (!IsAuthenticated(clientID)) return 332;
    auto currentDirectory = loggedInUsers[clientID].directory;
    auto dest = ConvertDirectory(currentDirectory, path);
    if (!PathExists(dest))
        return 500;
    loggedInUsers[clientID].directory = dest;
    return 250;
}

bool ServerCore::PathExists(vector<string> path)
{
    string dir = MakeDirStr(path);
    struct stat buffer;
    return (stat(dir.c_str(), &buffer) == 0);
}

GetDirectoryResponse ServerCore::GetCurrentDirectory(int clientID) {
    if (!IsAuthenticated(clientID)) return {332, ""};
    string dir = MakeDirStr(loggedInUsers[clientID].directory, false);
    return {257, dir};
}

int ServerCore::MakeDirectory(string path, int clientID)
{
    if (!IsAuthenticated(clientID)) return 332;
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

ShowListResponse ServerCore::ShowList(int clientID) {
    ShowListResponse showListResponse;
    if (!IsAuthenticated(clientID)) return {332, {}};
    auto dirStr = MakeDirStr(loggedInUsers[clientID].directory);
    for (auto entry : filesystem::directory_iterator(dirStr)) {
        showListResponse.names.push_back(entry.path().filename().string());
    }
    showListResponse.code = 226;
    return showListResponse;
}





