#include <string>
#include <vector>

#ifndef FTP_SERVER_SERVERAPI_HPP
#define FTP_SERVER_SERVERAPI_HPP

#define USER user
#define PASS pass
#define PWD pwd
#define MKD mkd
#define DELE dele
#define LS ls
#define CWD cwd
#define RENAME rename
#define RETR retr
#define HELP help
#define QUIT quit

class ServerAPI{
private:
    std::string CheckUsername(std::vector<std::string>);
    std::string Authenticate(std::vector<std::string>);
    std::string MakeDirectory(std::vector<std::string>);
    std::string DeleteFileOrDirectory(std::vector<std::string>);
    std::string ChangeDirectory(std::vector<std::string>);
    std::string RenameFile(std::vector<std::string>);
    std::string DownloadFile(std::vector<std::string>);
    std::string GetCurrentDirectory(std::vector<std::string>);
    std::string ShowList(std::vector<std::string>);
    std::string Help(std::vector<std::string>);
    std::string Quit(std::vector<std::string>);
public:
    ServerAPI();
    void Run();
    std::string HandleCommand(std::string);
};
#endif
