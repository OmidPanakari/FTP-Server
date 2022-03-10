#include "Logger.hpp"
#include <ctime>
#include <fstream>

class Logger{
private:
    std::ofstream outputFile;
    string GenerateTime(){
        time_t now = time(0);
        return ctime(&now);
    }
public:
    Logger(fileName){
        outputFile.open(fileName);
    }
    void Log(message){
        outputFile << message;
    }
};