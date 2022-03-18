#include "ServerCore.hpp"
#include "Logger.hpp"
#include "JsonSerializer.hpp"
#include <string>
#include <vector>

#ifndef FTP_SERVER_SERVERAPI_HPP
#define FTP_SERVER_SERVERAPI_HPP

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
#define CONFIGFILE "config.json"

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

class ServerAPI{
private:
    ServerCore* serverCore;
    JsonSerializer* jsonSerializer;
    Logger* logger;

    JsonSerializer MakeResponse(std::string, bool);
    std::string CheckUsername(std::vector<std::string>, int);
    std::string Authenticate(std::vector<std::string>, int);
    std::string MakeDirectory(std::vector<std::string>, int);
    std::string DeleteFileOrDirectory(std::vector<std::string>, int);
    std::string ChangeDirectory(std::vector<std::string>, int);
    std::string RenameFile(std::vector<std::string>, int);
    std::string DownloadFile(std::vector<std::string>, int);
    std::string GetCurrentDirectory(std::vector<std::string>, int);
    std::string ShowList(std::vector<std::string>, int);
    std::string Help(std::vector<std::string>, int);
    std::string Quit(std::vector<std::string>, int);

public:
    ServerAPI();
    void Run();
    std::string HandleCommand(std::string, int);
};
#endif
