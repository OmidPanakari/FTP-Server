#include <string>
#include <map>
#include <vector>
#include <set>
#include <iostream>
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
    std::vector<std::string> directory;
};

struct GetDirectoryResponse
{
    int code;
    std::string directory;
};


class ServerCore{
private:
    std::vector<User> users;
    std::set<std::string> adminFiles;
    std::map<int, OnlineUser> loggedInUsers;
    uint maxAllowedConnections;

    void ReadUsers(JsonSerializer);
    void ReadAdminFiles(JsonSerializer);
    bool IsAuthenticated(int);
    std::vector<std::string> Split(std::string, char);
    std::vector<std::string> ConvertDirectory(std::vector<std::string>, std::string);
    std::string MakeDirStr(std::vector<std::string>);
    bool PathExists(std::vector<std::string>);

public:
    ServerCore(JsonSerializer);
    int CheckUsername(std::string, int);
    int Authenticate(std::string, int);
    GetDirectoryResponse GetCurrentDirectory(int);
    int MakeDirectory(std::string, int);
    int DeleteFile(std::string, int);
    int DeleteDirectory(std::string, int);
    std::string ShowList(int);
    int ChangeDirectory(std::string, int);
    int RenameFile(std::string, std::string, int);
};
#endif
