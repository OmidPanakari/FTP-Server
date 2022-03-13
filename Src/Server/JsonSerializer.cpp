#include "JsonSerializer.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

void JsonSerializer::ReadJsonItem(string item)
{
    string key = "", value = "";
    int a, b;
    a = item.find(DQUOT);
    b = item.find(DQUOT, a + 1);
    key = item.substr(a + 1, b - a - 1);
    a = item.find(COL, b + 1);
    value = item.substr(a + 1);
    dictionary[key] = value;
}

void JsonSerializer::ReadJsonItems(string json)
{
    int curlyCnt = 0, bracCnt = 0, quotCnt = 0, i = 0;
    string item = "";
    while (i != json.size())
    {
        item += json[i];
        switch (json[i])
        {
        case LCURLY:
            curlyCnt++;
            break;
        case RCURLY:
            curlyCnt--;
            break;
        case LBRAC:
            bracCnt++;
            break;
        case RBRAC:
            bracCnt--;
            break;
        case DQUOT:
            quotCnt ^= 1;
            break;
        case COMMA:
            if (curlyCnt == 0 && bracCnt == 0 && quotCnt == 0)
            {
                ReadJsonItem(item);
                item = "";
            }
            break;
        default:
            break;
        }
        i++;
    }
    if (item.size())
        ReadJsonItem(item);
}

void JsonSerializer::ReadJson(string json)
{
    int start = -1, end = -1;
    for (int i = 0; i < json.size(); i++)
    {
        if (json[i] == LCURLY)
        {
            start = i;
            break;
        }
    }
    for (int i = json.size() - 1; i >= 0; i--)
    {
        if(json[i] == RCURLY)
        {
            end = i;
            break;
        }
    }
    ReadJsonItems(json.substr(start + 1, end - start - 1));
}

void JsonSerializer::ReadFile(string fileName)
{
    ostringstream inputStream;
    ifstream inputFile;;
    inputFile.open(fileName);
    inputStream << inputFile.rdbuf();
    string json = inputStream.str();
    ReadJson(json);
}

JsonSerializer JsonSerializer::GetSection(string key)
{
    JsonSerializer section;
    section.ReadJson(dictionary[key]);
    return section;
}

vector<JsonSerializer> JsonSerializer::ReadArrayItems(string json)
{
    int curlyCnt = 0, bracCnt = 0, quotCnt = 0, i = 0;
    string item = "";
    vector<JsonSerializer> result;
    while (i != json.size())
    {
        item += json[i];
        switch (json[i])
        {
        case LCURLY:
            curlyCnt++;
            break;
        case RCURLY:
            curlyCnt--;
            break;
        case LBRAC:
            bracCnt++;
            break;
        case RBRAC:
            bracCnt--;
            break;
        case DQUOT:
            quotCnt ^= 1;
            break;
        case COMMA:
            if (curlyCnt == 0 && bracCnt == 0 && quotCnt == 0)
            {
                JsonSerializer newItem;
                newItem.ReadJson(item);
                result.push_back(newItem);
                item = "";
            }
            break;
        default:
            break;
        }
        i++;
    }
    if (item.size())
    {
        JsonSerializer newItem;
        newItem.ReadJson(item);
        result.push_back(newItem);
    }
    return result;
}

vector<JsonSerializer> JsonSerializer::GetArray(string key)
{
    int start = -1, end = -1;
    string json = dictionary[key];
    for (int i = 0; i < json.size(); i++)
    {
        if (json[i] == LBRAC)
        {
            start = i;
            break;
        }
    }
    for (int i = json.size() - 1; i >= 0; i--)
    {
        if(json[i] == RBRAC)
        {
            end = i;
            break;
        }
    }
    return ReadArrayItems(json.substr(start + 1, end - start - 1));
}

string JsonSerializer::ReadValue(string quoted)
{
    int a = quoted.find(DQUOT);
    int b = quoted.find(DQUOT, a + 1);
    return quoted.substr(a + 1, b - a - 1);
}

string JsonSerializer::Get(string key)
{
    string value = dictionary[key];
    return ReadValue(value);
}

int JsonSerializer::GetInteger(string key)
{
    return stoi(Get(key));
}

vector<string> JsonSerializer::GetStrArray(string key)
{
    int start = -1, end = -1;
    string json = dictionary[key];
    for (int i = 0; i < json.size(); i++)
    {
        if (json[i] == LBRAC)
        {
            start = i;
            break;
        }
    }
    for (int i = json.size() - 1; i >= 0; i--)
    {
        if(json[i] == RBRAC)
        {
            end = i;
            break;
        }
    }
    return ReadStrArrayItems(json.substr(start + 1, end - start - 1));
}

vector<int> JsonSerializer::GetIntArray(string key)
{
    int start = -1, end = -1;
    string json = dictionary[key];
    for (int i = 0; i < json.size(); i++)
    {
        if (json[i] == LBRAC)
        {
            start = i;
            break;
        }
    }
    for (int i = json.size() - 1; i >= 0; i--)
    {
        if(json[i] == RBRAC)
        {
            end = i;
            break;
        }
    }
    return ReadIntArrayItems(json.substr(start + 1, end - start - 1));
}

vector<string> JsonSerializer::ReadStrArrayItems(string json)
{
    int curlyCnt = 0, bracCnt = 0, quotCnt = 0, i = 0;
    string item = "";
    vector<string> result;
    while (i != json.size())
    {
        item += json[i];
        switch (json[i])
        {
        case LCURLY:
            curlyCnt++;
            break;
        case RCURLY:
            curlyCnt--;
            break;
        case LBRAC:
            bracCnt++;
            break;
        case RBRAC:
            bracCnt--;
            break;
        case DQUOT:
            quotCnt ^= 1;
            break;
        case COMMA:
            if (curlyCnt == 0 && bracCnt == 0 && quotCnt == 0)
            {
                result.push_back(ReadValue(item));
                item = "";
            }
            break;
        default:
            break;
        }
        i++;
    }
    if (item.size())
    {
        result.push_back(ReadValue(item));
    }
    return result;
}

vector<int> JsonSerializer::ReadIntArrayItems(string json)
{
    vector<string> strVals = ReadStrArrayItems(json);
    vector<int> res;
    for (string s : strVals)
    {
        res.push_back(stoi(s));
    }
    return res;
}