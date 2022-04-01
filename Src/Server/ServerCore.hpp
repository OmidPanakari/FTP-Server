#include <string>
#include <map>
#include <vector>
#include <set>
#include <cstring>
#include <iostream>
#include "JsonSerializer.hpp"
#include "Logger.hpp"

#ifndef FTP_SERVER_SERVERCORE_HPP
#define FTP_SERVER_SERVERCORE_HPP

#define ROOT "./root"


struct User{
    std::string username;
    std::string password;
    bool isAdmin;
    int size;
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

struct ShowListResponse
{
    int code;
    std::vector<std::string> names;
};

struct GetFileResponse
{
    int code;
    std::string content;
};


class ServerCore{
private:
    std::vector<User> users;
    std::set<std::string> adminFiles;
    std::map<int, OnlineUser> loggedInUsers;
    uint maxAllowedConnections;
    Logger* logger;

    void ReadUsers(JsonSerializer*);
    void ReadAdminFiles(JsonSerializer*);
    bool IsAuthenticated(int);
    std::vector<std::string> MakeDirStk(std::vector<std::string>, std::string);
    std::string MakeDirStr(std::vector<std::string>, bool);
    bool PathExists(std::vector<std::string>);

public:
    ServerCore(JsonSerializer*, Logger*);
    int CheckUsername(std::string, int);
    int Authenticate(std::string, int);
    GetDirectoryResponse GetCurrentDirectory(int);
    int MakeDirectory(std::string, int);
    int DeleteFileOrDirectory(std::string, int);
    ShowListResponse ShowList(int);
    int ChangeDirectory(std::string, int);
    int RenameFile(std::string, std::string, int);
    GetFileResponse GetFile(std::string, int);
    int Quit(int);
};
#endif
