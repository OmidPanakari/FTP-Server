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

    void ReadJsonItems(std::string);
    void ReadJsonItem(std::string);
    std::vector<JsonSerializer> ReadArrayItems(std::string);
    std::vector<std::string> ReadStrArrayItems(std::string);
    std::vector<int> ReadIntArrayItems(std::string);
    std::string ReadValue(std::string);

public:
    void ReadJson(std::string);
    void ReadFile(std::string);
    JsonSerializer GetSection(std::string);
    std::vector<JsonSerializer> GetArray(std::string);
    std::string Get(std::string);
    int GetInteger(std::string);
    std::vector<std::string> GetStrArray(std::string);
    std::vector<int> GetIntArray(std::string);
    std::string GetJson();
    void AddItem(std::string, std::string);
    void AddList(std::string, std::vector<std::string>);
};

#endif