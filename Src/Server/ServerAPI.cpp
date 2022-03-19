#include "ServerAPI.hpp"
#include "Utility.hpp"


using namespace std;

ServerAPI::ServerAPI() {
    jsonSerializer = new JsonSerializer();
    jsonSerializer->ReadFile(CONFIGFILE);
    this->logger = new Logger(jsonSerializer->Get("logFile"));
    this->serverCore = new ServerCore(jsonSerializer, logger);
    maxAllowedConnections = jsonSerializer->GetInteger("maxAllowedConnections");
    requestSocket.port = jsonSerializer->GetInteger("requestPort");
    dataSocket.port = jsonSerializer->GetInteger("dataPort");
    newClientFD = -1;
}

JsonSerializer ServerAPI::MakeResponse(string message, bool isSuccess = false) {
    JsonSerializer js;
    js.AddItem("isSuccess", isSuccess ? "True" : "False");
    js.AddItem("message", message);
    return js;
}

string ServerAPI::HandleCommand(string command, int clientID) {
    vector<string> commandVector = Utility::Split(command, ' ');
    if (commandVector.size() < 1) return MakeResponse(SYNTAX_ERROR_MESSAGE).GetJson();
    if (commandVector[0] == USER){
        commandVector.erase(commandVector.begin());
        return CheckUsername(commandVector, clientID);
    } else if (commandVector[0] == PASS){
        commandVector.erase(commandVector.begin());
        return Authenticate(commandVector, clientID);
    } else if (commandVector[0] == PWD){
        commandVector.erase(commandVector.begin());
        return GetCurrentDirectory(commandVector, clientID);
    } else if (commandVector[0] == MKD){
        commandVector.erase(commandVector.begin());
        return MakeDirectory(commandVector, clientID);
    } else if(commandVector[0] == DELE){
        commandVector.erase(commandVector.begin());
        return DeleteFileOrDirectory(commandVector, clientID);
    } else if(commandVector[0] == LS){
        commandVector.erase(commandVector.begin());
        return ShowList(commandVector, clientID);
    } else if (commandVector[0] == CWD){
        commandVector.erase(commandVector.begin());
        return ChangeDirectory(commandVector, clientID);
    } else if(commandVector[0] == RENAME){
        commandVector.erase(commandVector.begin());
        return RenameFile(commandVector, clientID);
    } else if (commandVector[0] == RETR){
        commandVector.erase(commandVector.begin());
        return DownloadFile(commandVector, clientID);
    } else if (commandVector[0] == HELP){
        commandVector.erase(commandVector.begin());
        return Help(commandVector, clientID);
    } else if(commandVector[0] == QUIT){
        commandVector.erase(commandVector.begin());
        return Quit(commandVector, clientID);
    } else return MakeResponse(SYNTAX_ERROR_MESSAGE).GetJson();
}


string ServerAPI::CheckUsername(vector <string> command, int clientID) {
    if (command.size() != 1) return MakeResponse(SYNTAX_ERROR_MESSAGE).GetJson();
    int response = serverCore->CheckUsername(command[0], clientID);
    switch (response) {
        case 425:
            return MakeResponse(MAXIMUM_USERS_ERROR_MESSAGE).GetJson();
        case 331:
            return MakeResponse(USER_OK, true).GetJson();
        case 430:
            return MakeResponse(INVALID_CREDENTIALS_ERROR_MESSAGE).GetJson();
        default:
            return MakeResponse(UNKNOWN_ERROR_MESSAGE).GetJson();
    }
}

string ServerAPI::Authenticate(vector <string> command, int clientID) {
    if (command.size() != 1) return MakeResponse(SYNTAX_ERROR_MESSAGE).GetJson();
    int response = serverCore->Authenticate(command[0], clientID);
    switch (response) {
        case 230:
            return MakeResponse(USER_OK, true).GetJson();
        case 503:
            return MakeResponse(BAD_SEQUENCE_ERROR_MESSAGE).GetJson();
        case 430:
            return MakeResponse(INVALID_CREDENTIALS_ERROR_MESSAGE).GetJson();
        default:
            return MakeResponse(UNKNOWN_ERROR_MESSAGE).GetJson();
    }
}

string ServerAPI::GetCurrentDirectory(vector <string> command, int clientID) {
    if (command.size() != 0) return MakeResponse(SYNTAX_ERROR_MESSAGE).GetJson();
    GetDirectoryResponse response = serverCore->GetCurrentDirectory(clientID);
    switch (response.code) {
        case 332:
            return MakeResponse(UNAUTHORIZED_ERROR_MESSAGE).GetJson();
        case 257:
            return MakeResponse("257: "+response.directory, true).GetJson();
        default:
            return MakeResponse(UNKNOWN_ERROR_MESSAGE).GetJson();
    }
}

string ServerAPI::MakeDirectory(vector <string> command, int clientID) {
    if (command.size() != 1) return MakeResponse(SYNTAX_ERROR_MESSAGE).GetJson();
    int response = serverCore->MakeDirectory(command[0], clientID);
    switch (response) {
        case 332:
            return MakeResponse(UNAUTHORIZED_ERROR_MESSAGE).GetJson();
        case 257:
            return MakeResponse("257: "+command[0] + " created.", true).GetJson();
        default:
            return MakeResponse(UNKNOWN_ERROR_MESSAGE).GetJson();
    }
}

string ServerAPI::DeleteFileOrDirectory(vector <string> command, int clientID) {
    if (command.size() != 2) return MakeResponse(SYNTAX_ERROR_MESSAGE).GetJson();
    if (command[0] != "-d" && command[0] != "-f") return MakeResponse(SYNTAX_ERROR_MESSAGE).GetJson();
    int response = serverCore->MakeDirectory(command[0], clientID);
    switch (response) {
        case 332:
            return MakeResponse(UNAUTHORIZED_ERROR_MESSAGE).GetJson();
        case 250:
            return MakeResponse("250: " + command[0] + " deleted.", true).GetJson();
        case 550:
            return MakeResponse(PERMISSION_DENIED_ERROR_MESSAGE).GetJson();
        default:
            return MakeResponse(UNKNOWN_ERROR_MESSAGE).GetJson();
    }
}

string ServerAPI::ShowList(vector <string> command, int clientID) {
    if (command.size() != 0) return MakeResponse(SYNTAX_ERROR_MESSAGE).GetJson();
    ShowListResponse response = serverCore->ShowList(clientID);
    switch (response.code) {
        case 332:
            return MakeResponse(UNAUTHORIZED_ERROR_MESSAGE).GetJson();
        case 226:
            return MakeResponse(LIST_TRANSFER_OK, true).GetJson();
        default:
            return MakeResponse(UNKNOWN_ERROR_MESSAGE).GetJson();
    }
    //TODO:send list data through channel
}

string ServerAPI::ChangeDirectory(vector <string> command, int clientID) {
    if (command.size() != 1) return MakeResponse(SYNTAX_ERROR_MESSAGE).GetJson();
    int response = serverCore->ChangeDirectory(command[0], clientID);
    switch (response) {
        case 332:
            return MakeResponse(UNAUTHORIZED_ERROR_MESSAGE).GetJson();
        case 250:
            return MakeResponse("250: Successful change.", true).GetJson();
        default:
            return MakeResponse(UNKNOWN_ERROR_MESSAGE).GetJson();
    }
}

string ServerAPI::RenameFile(vector <string> command, int clientID) {
    if (command.size() != 2) return MakeResponse(SYNTAX_ERROR_MESSAGE).GetJson();
    int response = serverCore->RenameFile(command[0], command[1], clientID);
    switch (response) {
        case 332:
            return MakeResponse(UNAUTHORIZED_ERROR_MESSAGE).GetJson();
        case 250:
            return MakeResponse("250: Successful change.", true).GetJson();
        case 550:
            return MakeResponse(PERMISSION_DENIED_ERROR_MESSAGE).GetJson();
        default:
            return MakeResponse(UNKNOWN_ERROR_MESSAGE).GetJson();
    }
}

string ServerAPI::DownloadFile(vector <string> command, int clientID) {
    if (command.size() != 1) return MakeResponse(SYNTAX_ERROR_MESSAGE).GetJson();
}

string ServerAPI::Help(vector <string> command, int clientID) {
    if (command.size() != 0) return MakeResponse(SYNTAX_ERROR_MESSAGE).GetJson();
}

string ServerAPI::Quit(vector <string> command, int clientID) {
    if (command.size() != 0) return MakeResponse(SYNTAX_ERROR_MESSAGE).GetJson();
    int response = serverCore->Quit(clientID);
    switch (response) {
        case 332:
            return MakeResponse(UNAUTHORIZED_ERROR_MESSAGE).GetJson();
        case 250:
            return MakeResponse(QUIT_OK, true).GetJson();
        default:
            return MakeResponse(UNKNOWN_ERROR_MESSAGE).GetJson();
    }
}

void ServerAPI::SetupSockets() {
    requestSocket.FD = socket(AF_INET, SOCK_STREAM, 0);
    dataSocket.FD = socket(AF_INET, SOCK_STREAM, 0);
    
    int opt = 1;
    setsockopt(requestSocket.FD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    setsockopt(dataSocket.FD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    requestSocket.address.sin_family = AF_INET;
    requestSocket.address.sin_addr.s_addr = INADDR_ANY;
    requestSocket.address.sin_port = htons(requestSocket.port); 
    dataSocket.address.sin_family = AF_INET;
    dataSocket.address.sin_addr.s_addr = INADDR_ANY;
    dataSocket.address.sin_port = htons(dataSocket.port); 

    bind(requestSocket.FD, (struct  sockaddr *)&requestSocket.address, sizeof(requestSocket.address));
    bind(dataSocket.FD, (struct  sockaddr *)&dataSocket.address, sizeof(dataSocket.address));
}

void ServerAPI::StartListening() {
    listen(requestSocket.FD, maxAllowedConnections);
    listen(dataSocket.FD, maxAllowedConnections);

    FD_ZERO(&(requestSocket.readSet));
    FD_ZERO(&(requestSocket.writeSet));
    FD_ZERO(&(dataSocket.readSet));
    FD_ZERO(&(dataSocket.writeSet));
    FD_SET(requestSocket.FD, &(requestSocket.readSet));
    FD_SET(dataSocket.FD, &(dataSocket.readSet));
}

void ServerAPI::HandleRequests() {
    for (int i = 0; i < requestSocket.maxFD; i++)
    {
        if (FD_ISSET(i, &(requestSocket.workingReadSet))) {
            if (i == requestSocket.FD) {
                if (newClientFD == -1) {
                    struct sockaddr_in clientAddress;
                    int addressLen = sizeof(clientAddress);
                    newClientFD = accept(requestSocket.FD, (struct sockaddr *)&clientAddress, (socklen_t *)&addressLen);
                    clients.insert(newClientFD);
                }
            }
            else {
                
            }
        }
    }
}

void ServerAPI::Run() {
    logger->Log("Initializing server.");
    SetupSockets();
    StartListening();
    logger->Log("Started listening for clients");
    while (true) {
        requestSocket.workingReadSet = requestSocket.readSet;
        dataSocket.workingReadSet = dataSocket.readSet;
        requestSocket.workingWriteSet = requestSocket.writeSet;
        dataSocket.workingWriteSet = dataSocket.writeSet;
        select(requestSocket.maxFD + 1, &(requestSocket.workingReadSet), &(requestSocket.workingWriteSet), NULL, NULL);
        HandleRequests();
        select(dataSocket.maxFD + 1, &(dataSocket.workingReadSet), &(dataSocket.workingWriteSet), NULL, NULL);
    }
}