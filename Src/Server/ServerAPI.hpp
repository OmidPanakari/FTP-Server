#include <string>

#ifndef FTP_SERVER_SERVERAPI_HPP
#define FTP_SERVER_SERVERAPI_HPP
class ServerAPI{
public:
    ServerAPI();
    void Run();
    std::string HandleCommand(std::string);
};
#endif
