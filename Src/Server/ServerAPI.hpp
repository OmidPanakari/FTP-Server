#include "ServerCore.hpp"
#include "Logger.hpp"
#include "JsonSerializer.hpp"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

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
#define QUIT "quit"
#define CONFIGFILE "config.json"

#define MAX_BUF_SIZE 2048

struct Socket {
    int port, FD;
    struct sockaddr_in address;
};

struct Client {
    int dataFD, fileLen;
    std::string message;
    std::vector<std::string> contentParts;
    bool isDownloading;
};

class ServerAPI{
private:
    ServerCore* serverCore;
    JsonSerializer* jsonSerializer;
    Logger* logger;
    uint maxAllowedConnections;
    std::map<int, Client> clients;
    std::set<int> unmappedClients;
    Socket requestSocket, dataSocket;
    int maxFD;
    fd_set readSet, writeSet, workingReadSet, workingWriteSet;
    char buf[MAX_BUF_SIZE + 1];

    JsonSerializer MakeResponse(std::string, bool);
    std::string CheckUsername(JsonSerializer, int);
    std::string Authenticate(JsonSerializer, int);
    std::string MakeDirectory(JsonSerializer, int);
    std::string DeleteFileOrDirectory(JsonSerializer, int);
    std::string ChangeDirectory(JsonSerializer, int);
    std::string RenameFile(JsonSerializer, int);
    std::string DownloadFile(JsonSerializer, int);
    std::string GetCurrentDirectory(int);
    std::string ShowList(int);
    std::string Quit(int);

    void SetupSockets();
    void StartListening();
    void HandleRequests();
    void AcceptClient();
    void AcceptDataClient();
    void GetRequest(int);
    void SendMessage(int);
    void MapDataClient(int);
    void StartDownload(int);
    void SendData(int);
    void HandleDownloads();

public:
    ServerAPI();
    void Run();
    std::string HandleRequest(std::string, int);
};
#endif
