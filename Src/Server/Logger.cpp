#include "Logger.hpp"
#include <ctime>

using namespace std;

Logger::Logger(string fileName){
    this->fileName = fileName;
}

void Logger::Log(string message){
    ofstream logFile(fileName, ios::app);
    string date = GenerateTime();
    date[date.size() - 1] = ' ';
    logFile << date + ": " + message + "\n";
    logFile.close();

}
string Logger::GenerateTime(){
    time_t now = time(0);
    return ctime(&now);
}
