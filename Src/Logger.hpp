#include <string.h>

#ifndef FTP_SERVER_LOGGER_HPP
#define FTP_SERVER_LOGGER_HPP
class Logger{
public:
    Logger(string fileName);
    void Log(string message);
};
#endif
