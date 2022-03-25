#include <string>
#include <fstream>

#ifndef FTP_SERVER_LOGGER_HPP
#define FTP_SERVER_LOGGER_HPP
class Logger {
private:
    std::string fileName;
    
    std::string GenerateTime();
public:
    Logger(std::string);
    void Log(std::string);
};
#endif
