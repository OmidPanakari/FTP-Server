#include <string>
#include <map>
#include <vector>
#include "JsonSerializer.hpp"

#ifndef FTP_SERVER_SERVERCORE_HPP
#define FTP_SERVER_SERVERCORE_HPP


struct User{
    std::string username;
    std::string password;
    bool isAdmin;
};

struct OnlineUser
{
    User *user;
    bool isAuthenticated; 
};

struct LoginResponse
{
    int token;
    int code;
};


class ServerCore{
private:
    std::vector<User> users;
    std::vector<std::string> adminFiles;
    std::map<int, OnlineUser> loggedInUsers;
    std::string currentDirectory;
    int maxAllowedConnections;
    int lastToken;

    void ReadUsers(JsonSerializer);
    void ReadAdminFiles(JsonSerializer);

public:
    ServerCore(JsonSerializer);
    LoginResponse CheckUsername(std::string);
    int Authenticate(std::string, std::string);
    int GetCurrentDirectory();
    int MakeDirectory(std::string);
    int DeleteFile(std::string);
    int DeleteDirectory(std::string);
    int DeleteFile(std::string);
    std::string ShowList();
    int ChangeDirectory(std::string);
    int RenameFile(std::string, std::string);
};
#endif
