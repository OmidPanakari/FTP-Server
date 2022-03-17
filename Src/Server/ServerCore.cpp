#include "ServerCore.hpp"

using namespace std;

ServerCore::ServerCore(JsonSerializer config)
{
    ReadUsers(config);
    ReadAdminFiles(config);
    maxAllowedConnections = config.GetInteger("maxAllowedConnections");
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
            onlineUser->directory = "/";
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

string ServerCore::ConvertDirectory(string current, string path) {
    vector<string> pathWay = Split(path, '/');
    vector<string> currentWay = Split(current, '/');
    int pointer;
    if (path[0] == '/') pointer = 0;
    else pointer = currentWay.size() - 1;
    for (int i = 0; i < pathWay.size() - 1; i++) {
        if (pathWay[i] == ".." && pointer > 0){
            pointer--;
        } else if (pathWay[i] != "."){
            if (pointer == currentWay.size() - 1){
                currentWay.push_back(pathWay[i]);
                pointer++;
            } else {
                currentWay[i] = pathWay[i];
                pointer++;
            }

        }
    }


}
GetDirectoryResponse ServerCore::GetCurrentDirectory(int clientID) {
    if (!IsAuthenticated(clientID)) return 332;
    return {257, loggedInUsers[clientID].directory};
}






