#include "ServerAPI.hpp"
#include "Utility.hpp"

using namespace std;


string ServerAPI::HandleCommand(string command) {
    vector<string> commandVector = Split(command,' ');
    if (commandVector.size() < 1) return "501";
    if (commandVector[0] == USER){
        commandVector.erase();
        return CheckUsername(commandVector);
    } else if (commandVector[0] == PASS){
        commandVector.erase();
        return Authenticate(commandVector);
    } else if (commandVector[0] == PWD){
        commandVector.erase();
        return GetCurrentDirectory(commandVector);
    } else if (commandVector[0] == MKD){
        commandVector.erase();
        return MakeDirectory(commandVector);
    } else if(commandVector[0] == DELE){
        commandVector.erase();
        return DeleteFileOrDirectory(commandVector);
    } else if(commandVector[0] == LS){
        commandVector.erase();
        return ShowList(commandVector);
    } else if (commandVector[0] == CWD){
        commandVector.erase();
        return ChangeDirectory(commandVector);
    } else if(commandVector[0] == RENAME){
        commandVector.erase();
        return RenameFile(commandVector);
    } else if (commandVector[0] == RETR){
        commandVector.erase();
        return DownloadFile(commandVector);
    } else if (commandVector[0] == HELP){
        commandVector.erase();
        return Help(commandVector);
    } else if(commandVector[0] == QUIT){
        commandVector.erase();
        return Quit(commandVector);
    } else return "501";
}

string ServerAPI::CheckUsername(vector <string> command) {
    if (command.size() != 1) return "501";

}

string ServerAPI::Authenticate(vector <string> command) {
    if (command.size() != 1) return "501";
}

string ServerAPI::GetCurrentDirectory(vector <string> command) {
    if (command.size() != 0) return "501";
}

string ServerAPI::MakeDirectory(vector <string> command) {
    if (command.size() != 1) return "501";
}

string ServerAPI::DeleteFileOrDirectory(vector <string> command) {
    if (command.size() != 2) return "501";
    if (command[0] != "-d" && command[0] != "-f") return "501";
}

string ServerAPI::ShowList(vector <string> command) {
    if (command.size() != 0) return "501";
}

string ServerAPI::ChangeDirectory(vector <string> command) {
    if (command.size() != 1) return "501";
}

string ServerAPI::RenameFile(vector <string> command) {
    if (command.size() != 2) return "501";
}

string ServerAPI::DownloadFile(vector <string> command) {
    if (command.size() != 1) return "501";
}

string ServerAPI::Help(vector <string> command) {
    if (command.size() != 0) return "501";
}

string ServerAPI::Quit(vector <string> command) {
    if (command.size() != 0) return "501";
}