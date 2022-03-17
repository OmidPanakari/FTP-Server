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
    adminFiles = config.GetStrArray("privilegedFiles");
}

LoginResponse ServerCore::CheckUsername(std::string username) {
    if (loggedInUsers.size() > maxAllowedConnections)
        return {-1, 501};
    for (int i = 0; i < users.size(); i++) {
        if (users[i].username == username)
        {
            OnlineUser newLogin;
            newLogin.user = &users[i];
            newLogin.isAuthenticated = false;
            loggedInUsers[lastToken] = newLogin;
            return {lastToken, 331};
        }
    }
    return {-1, 331};
}




