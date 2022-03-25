#include <string>
#include <vector>

#ifndef FTP_SERVER_UTILITY_HPP
#define FTP_SERVER_UTILITY_HPP

class Utility{
public:
    std::vector<std::string> static Split(std::string, char);
    std::string static ToStr(int);
};

#endif
