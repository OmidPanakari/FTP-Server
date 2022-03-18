#include "ServerCore.hpp"
#include "Logger.hpp"
#include <sys/stat.h>
#include <filesystem>

using namespace std;

ServerCore::ServerCore(JsonSerializer config)
    :logger(LOG_FILE) {
    ReadUsers(config);
    ReadAdminFiles(config);
    maxAllowedConnections = config.GetInteger("maxAllowedConnections");
    struct stat buffer;
    if (stat(ROOT, &buffer) != 0) {
        mkdir(ROOT, 0777);
    }
}

void ServerCore::ReadUsers(JsonSerializer config) {
    users.clear();
    for (auto userConfig : config.GetArray("users")) {
        User newUser;
        newUser.username = userConfig.Get("username");
        newUser.password = userConfig.Get("password");
        newUser.isAdmin = userConfig.Get("isAdmin") == "True" ? true : false;
        users.push_back(newUser);        
    }
}

void ServerCore::ReadAdminFiles(JsonSerializer config) {
    for (auto file : config.GetStrArray("privilegedFiles")) {
        adminFiles.insert(file);
    };
}

bool ServerCore::IsAuthenticated(int clientID) {
    auto user = loggedInUsers.find(clientID);
    if(user != loggedInUsers.end()){
        if (loggedInUsers[clientID].isAuthenticated) {
            return true;
        }
    }
    return false;
}

vector <string> ServerCore::Split(string str, char c) {
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

string ServerCore::MakeDirStr(vector<string> pathStk, bool addRoot = true) {
    string dir = "";
    for (string p : pathStk) {
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
    for (string p : pathWay) {
        if (p == "..") {
            if (current.size())
                current.pop_back();
        }
        else if (p != ".") {
            current.push_back(p);
        }
    }
    return current;
}

bool ServerCore::PathExists(vector<string> path) {
    string dir = MakeDirStr(path);
    struct stat buffer;
    return (stat(dir.c_str(), &buffer) == 0);
}

//----Command Handlers-----

int ServerCore::CheckUsername(string username, int clientID) {
    if (loggedInUsers.size() > maxAllowedConnections) {
        logger.Log("Maximum number of users exeeded.");
        return 501;
    }
    for (uint i = 0; i < users.size(); i++) {
        if (users[i].username == username) {
            OnlineUser newLogin;
            newLogin.user = &users[i];
            newLogin.isAuthenticated = false;
            loggedInUsers[clientID] = newLogin;
            logger.Log("Username " + username + " was checked.");
            return 331;
        }
    }
    logger.Log("Invalid username.");
    return 430;
}

int ServerCore::Authenticate(string password, int clientID) {
    auto user = loggedInUsers.find(clientID);
    if(user != loggedInUsers.end()) {
        OnlineUser* onlineUser = &user->second;
        if(onlineUser->user->password == password) {
            onlineUser->isAuthenticated = true;
            onlineUser->directory.clear();
            cout << loggedInUsers[clientID].isAuthenticated << endl;
            logger.Log("User " + onlineUser->user->username + " authenticated.");
            return 230;
        }
        logger.Log("Invalid password for user " + onlineUser->user->username +".");
        return 430;
    }
    return 503;
}

GetDirectoryResponse ServerCore::GetCurrentDirectory(int clientID) {
    if (!IsAuthenticated(clientID)) {
        logger.Log("Unauthorized request.");
        return {332, ""};
    }
    string dir = MakeDirStr(loggedInUsers[clientID].directory, false);
    logger.Log("User " + loggedInUsers[clientID].user->username + " viewed current directory -> " + dir + ".");
    return {257, dir};
}

int ServerCore::MakeDirectory(string path, int clientID) {
    if (!IsAuthenticated(clientID)) {
        logger.Log("Unauthorized request.");
        return 332;
    }
    auto dest = ConvertDirectory(loggedInUsers[clientID].directory, path);
    if (dest.empty())
        return 500;
    string newDir = dest.back();
    if (PathExists(dest)) {
        logger.Log("User " + loggedInUsers[clientID].user->username + " wanted to create already existing directory -> " + path + ".");
        return 500;
    }
    dest.pop_back();
    if (!PathExists(dest)) {
        logger.Log("User " + loggedInUsers[clientID].user->username + " desired path doesn't exist.");
        return 500;
    }
    dest.push_back(newDir);
    if (mkdir(MakeDirStr(dest).c_str(), 0777) == 0) {
        logger.Log("User " + loggedInUsers[clientID].user->username + " created new directory -> " + path + ".");
        return 257;
    }
    logger.Log("Unknow error.");
    return 500;
}

int ServerCore::DeleteFileOrDirectory(std::string path, int clientID) {
    if (!IsAuthenticated(clientID)) {
        logger.Log("Unauthorized request.");
        return 332;
    }
    auto currentDirectory = loggedInUsers[clientID].directory;
    auto dest = ConvertDirectory(currentDirectory, path);
    auto destStr = MakeDirStr(dest);
    if (!PathExists(dest)) {
        logger.Log("User " + loggedInUsers[clientID].user->username + " desired path doesn't exist.");
        return 500;
    }
    if (!loggedInUsers[clientID].user->isAdmin && adminFiles.find(destStr) != adminFiles.end()) {
        logger.Log("User " + loggedInUsers[clientID].user->username + " doesn't have sufficient permission to access desired file -> " + path + ".");
        return 550;
    }
    if (remove(destStr.c_str()) == 0) {
        logger.Log("User " + loggedInUsers[clientID].user->username + " deleted file/directory -> " + path + ".");
        return 250;
    }
    logger.Log("Unknow error.");
    return 500;
}

ShowListResponse ServerCore::ShowList(int clientID) {
    ShowListResponse showListResponse;
    if (!IsAuthenticated(clientID)) {
        logger.Log("Unauthorized request.");
        return {332, {}};
    }
    auto dirStr = MakeDirStr(loggedInUsers[clientID].directory);
    if (!PathExists(loggedInUsers[clientID].directory)) {
        logger.Log("User " + loggedInUsers[clientID].user->username + " desired path doesn't exist.");
        return {500, {}};
    }
    for (auto entry : filesystem::directory_iterator(dirStr)) {
        showListResponse.names.push_back(entry.path().filename().string());
    }
    showListResponse.code = 226;
    logger.Log("User " + loggedInUsers[clientID].user->username + "viewed list of entries in directory " + dirStr + ".");
    return showListResponse;
}

int ServerCore::ChangeDirectory(std::string path, int clientID) {
    if (!IsAuthenticated(clientID)) {
        logger.Log("Unauthorized request.");
        return 332;
    }
    auto currentDirectory = loggedInUsers[clientID].directory;
    auto dest = ConvertDirectory(currentDirectory, path);
    if (!PathExists(dest)) {
        logger.Log("User " + loggedInUsers[clientID].user->username + " desired path doesn't exist.");
        return 500;
    }
    loggedInUsers[clientID].directory = dest;
    logger.Log("User " + loggedInUsers[clientID].user->username + " changed directory from " + MakeDirStr(currentDirectory) + " to " + MakeDirStr(dest) + ".");
    return 250;
}

int ServerCore::RenameFile(std::string path, std::string newPath, int clientID) {
    if (!IsAuthenticated(clientID)) {
        logger.Log("Unauthorized request.");
        return 332;
    }
    auto currentDirectory = loggedInUsers[clientID].directory;
    auto dest = ConvertDirectory(currentDirectory, path);
    auto old = dest.back();
    if (!PathExists(dest)) {
        logger.Log("User " + loggedInUsers[clientID].user->username + " desired path doesn't exist.");
        return 500;
    }
    dest.pop_back();
    auto destStr = MakeDirStr(dest) + "/";
    if (!loggedInUsers[clientID].user->isAdmin && adminFiles.find(destStr + old) != adminFiles.end()) {
        logger.Log("User " + loggedInUsers[clientID].user->username + " doesn't have sufficient permission to access desired file -> " + path + ".");
        return 550;
    }
    if (rename((destStr + old).c_str(), (destStr + newPath).c_str()) == 0) {
        logger.Log("User " + loggedInUsers[clientID].user->username + " renamed " + path + " to " + newPath + ".");
        return 250;
    }
    logger.Log("Unknow error.");
    return 500;
}

int ServerCore::Quit(int clientID) {
    if (!IsAuthenticated(clientID)) {
        logger.Log("Unauthorized request.");
        return 332;
    }
    loggedInUsers.erase(clientID);
    logger.Log("User " + loggedInUsers[clientID].user->username + " logged out.");
    return 221;
}