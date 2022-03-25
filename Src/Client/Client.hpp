#include "JsonSerializer.hpp"
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>


#ifndef FTP_SERVER_CLIENT_HPP
#define FTP_SERVER_CLIENT_HPP static

#define CONFIGFILE "config.json"
#define DOWNLOAD_DIR "./download"
#define MAX_BUF_SIZE 1024

#define USER "user"
#define PASS "pass"
#define PWD "pwd"
#define MKD "mkd"
#define DELE "dele"
#define LS "ls"
#define CWD "cwd"
#define RENAME "rename"
#define RETR "retr"
#define HELP "help"
#define QUIT "quit"

#define SYNTAX_ERROR_MESSAGE "501: Syntax error in parameters or arguments."
#define MAXIMUM_USERS_ERROR_MESSAGE "425: Can't open data connection."
#define USER_OK "331: User name okay, need password."
#define INVALID_CREDENTIALS_ERROR_MESSAGE "430: Invalid username or password."
#define UNKNOWN_ERROR_MESSAGE "500: Error"
#define BAD_SEQUENCE_ERROR_MESSAGE "503: Bad sequence of commands."
#define LOGIN_OK "230: User logged in, proceed. Logged out if appropriate."
#define LIST_TRANSFER_OK "226: List transfer done."
#define QUIT_OK "221: Successful Quit."
#define UNAUTHORIZED_ERROR_MESSAGE "332: Need account for login."
#define PERMISSION_DENIED_ERROR_MESSAGE "550: File unavailable."
#define CHANGE_DIRECTORY_OK "250: Successful change."
#define CHANGE_NAME_OK "250: Successful change."
#define DOWNLOAD_OK "226: Successful Download."

class Client {
private:
    int requestPort, dataPort;
    int requestFD, dataFD;
    char buf[MAX_BUF_SIZE];
    
    void ConnectServer();
    void HandleCommand(std::string);
    void DownloadFile(std::string);

    void CheckUsername(std::vector<std::string>);
    void Authenticate(std::vector<std::string>);
    void MakeDirectory(std::vector<std::string>);
    void DeleteFileOrDirectory(std::vector<std::string>);
    void ChangeDirectory(std::vector<std::string>);
    void RenameFile(std::vector<std::string>);
    void GetFile(std::vector<std::string>);
    void GetCurrentDirectory(std::vector<std::string>);
    void ShowList(std::vector<std::string>);
    void Help(std::vector<std::string>);
    void Quit(std::vector<std::string>);

public:
    Client();
    void Run();
};

#endif