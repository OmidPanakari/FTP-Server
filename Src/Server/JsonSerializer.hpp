#include <string>
#include <map>
#include <vector>

#ifndef FTP_SERVER_JSON_SERIALIZER_HPP
#define FTP_SERVER_JSON_SERIALIZER_HPP

#define LCURLY '{'
#define RCURLY '}'
#define LBRAC '['
#define RBRAC ']'
#define DQUOT '\"'
#define COMMA ','
#define COL ':'

class JsonSerializer{
private:
    std::map<std::string, std::string> dictionary;

    bool IsWS(char);
    void ReadJsonItems(std::string);
    void ReadJsonItem(std::string);
    std::vector<JsonSerializer> ReadArrayItems(std::string);

public:
    void ReadJson(std::string);
    void ReadFile(std::string);
    JsonSerializer GetSection(std::string);
    std::vector<JsonSerializer> GetArray(std::string);
    std::string Get(std::string);
    int GetInteger(std::string);
};

#endif