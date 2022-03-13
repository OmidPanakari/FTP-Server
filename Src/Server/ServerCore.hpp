#include <string>
#include <map>
#include <vector>

#ifndef FTP_SERVER_SERVERCORE_HPP
#define FTP_SERVER_SERVERCORE_HPP


struct User{
    std::string username;
    std::string password;
};
class ServerCore{
private:
    std::map<int><std::string> loggedInUsers;
    std::string currentDirectory;
public:
    ServerCore(std::vector<User>, std::vector<User>, std::vector<std::string>);
    int CheckUsername(std::string);
    int Authenticate(std::string, std::string);
    int GetDirectory();
    int MakeDirectory(std::string);
    int DeleteFile(std::string);
    int DeleteDirectory(std::string);
    int DeleteFile(std::string);
    std::string ShowList();
    int ChangeDirectory(std::string);
    int RenameFile(std::string, std::string);

};
#endif
