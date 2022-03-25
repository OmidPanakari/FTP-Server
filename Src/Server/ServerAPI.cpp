#include "ServerAPI.hpp"
#include "Utility.hpp"
#include <cstring>


using namespace std;

ServerAPI::ServerAPI() {
    jsonSerializer = new JsonSerializer();
    jsonSerializer->ReadFile(CONFIGFILE);
    this->logger = new Logger(jsonSerializer->Get("logFile"));
    this->serverCore = new ServerCore(jsonSerializer, logger);
    maxAllowedConnections = jsonSerializer->GetInteger("maxAllowedConnections");
    requestSocket.port = jsonSerializer->GetInteger("requestPort");
    dataSocket.port = jsonSerializer->GetInteger("dataPort");
}

JsonSerializer ServerAPI::MakeResponse(string message, bool isSuccess = false) {
    JsonSerializer js;
    js.AddItem("isSuccess", isSuccess ? "True" : "False");
    js.AddItem("message", message);
    return js;
}

string ServerAPI::HandleRequest(string request, int clientID) {
    try {
        JsonSerializer requestDeserializer;
        requestDeserializer.ReadJson(request);
        if (requestDeserializer.Get("method") == USER) {
            return CheckUsername(requestDeserializer, clientID);
        } else if (requestDeserializer.Get("method") == PASS) {
            return Authenticate(requestDeserializer, clientID);
        } else if (requestDeserializer.Get("method") == PWD) {
            return GetCurrentDirectory(clientID);
        } else if (requestDeserializer.Get("method") == MKD){
            return MakeDirectory(requestDeserializer, clientID);
        } else if(requestDeserializer.Get("method") == DELE){
            return DeleteFileOrDirectory(requestDeserializer, clientID);
        } else if(requestDeserializer.Get("method") == LS){
            return ShowList(clientID);
        } else if (requestDeserializer.Get("method") == CWD){
            return ChangeDirectory(requestDeserializer, clientID);
        } else if(requestDeserializer.Get("method") == RENAME){
            return RenameFile(requestDeserializer, clientID);
        } else if (requestDeserializer.Get("method") == RETR){
            return DownloadFile(requestDeserializer, clientID);
        } else if(requestDeserializer.Get("method") == QUIT){
            return Quit(clientID);
        } else {
            JsonSerializer responseSerializer;
            responseSerializer.AddItem("code", "500");
            return responseSerializer.GetJson();
        }
    }
    catch (...) {
        JsonSerializer responseSerializer;
        responseSerializer.AddItem("code", "500");
        return responseSerializer.GetJson();
    }
}


string ServerAPI::CheckUsername(JsonSerializer requestDeserializer, int clientID) {
    int response = serverCore->CheckUsername(requestDeserializer.Get("username"), clientID);
    JsonSerializer responseSerializer;
    responseSerializer.AddItem("code", Utility::ToStr(response));
    return responseSerializer.GetJson();
}

string ServerAPI::Authenticate(JsonSerializer requestDeserializer, int clientID) {
    int response = serverCore->Authenticate(requestDeserializer.Get("password"), clientID);
    JsonSerializer responseSerializer;
    responseSerializer.AddItem("code", Utility::ToStr(response));
    return responseSerializer.GetJson();
}

string ServerAPI::GetCurrentDirectory(int clientID) {
    GetDirectoryResponse response = serverCore->GetCurrentDirectory(clientID);
    JsonSerializer responseSerializer;
    responseSerializer.AddItem("code", Utility::ToStr(response.code));
    responseSerializer.AddItem("directory", response.directory);
    return responseSerializer.GetJson();
}

string ServerAPI::MakeDirectory(JsonSerializer requestDeserializer, int clientID) {
    int response = serverCore->MakeDirectory(requestDeserializer.Get("path"), clientID);
    JsonSerializer responseSerializer;
    responseSerializer.AddItem("code", Utility::ToStr(response));
    return responseSerializer.GetJson();
}

string ServerAPI::DeleteFileOrDirectory(JsonSerializer requestDeserializer, int clientID) {
    int response = serverCore->DeleteFileOrDirectory(requestDeserializer.Get("path"), clientID);
    JsonSerializer responseSerializer;
    responseSerializer.AddItem("code", Utility::ToStr(response));
    return responseSerializer.GetJson();
}

string ServerAPI::ShowList(int clientID) {
    ShowListResponse response = serverCore->ShowList(clientID);
    JsonSerializer responseSerializer;
    responseSerializer.AddItem("code", Utility::ToStr(response.code));
    responseSerializer.AddList("names", response.names);
    return responseSerializer.GetJson();
}

string ServerAPI::ChangeDirectory(JsonSerializer requestDeserializer, int clientID) {
    int response = serverCore->ChangeDirectory(requestDeserializer.Get("path"), clientID);
    JsonSerializer responseSerializer;
    responseSerializer.AddItem("code", Utility::ToStr(response));
    return responseSerializer.GetJson();
}

string ServerAPI::RenameFile(JsonSerializer requestDeserializer, int clientID) {
    int response = serverCore->RenameFile(requestDeserializer.Get("path"), requestDeserializer.Get("newPath"), clientID);
    JsonSerializer responseSerializer;
    responseSerializer.AddItem("code", Utility::ToStr(response));
    return responseSerializer.GetJson();
}

string ServerAPI::DownloadFile(JsonSerializer requestDeserializer, int clientID) {
    GetFileResponse response = serverCore->GetFile(requestDeserializer.Get("filename"), clientID);
    JsonSerializer responseSerializer;
    responseSerializer.AddItem("code", Utility::ToStr(response.code));
    clients[clientID].isDownloading = true;
    vector<string> contentParts;
    for (uint i = 0; i < response.content.size(); i += MAX_BUF_SIZE) {
        contentParts.push_back(response.content.substr(i, MAX_BUF_SIZE));
    }
    clients[clientID].contentParts = contentParts;
    clients[clientID].fileLen = strlen(response.content.c_str());
    return responseSerializer.GetJson();
}

string ServerAPI::Quit(int clientID) {
    int response = serverCore->Quit(clientID);
    JsonSerializer responseSerializer;
    responseSerializer.AddItem("code", Utility::ToStr(response));
    return responseSerializer.GetJson();
}

void ServerAPI::SetupSockets() {
    requestSocket.FD = socket(AF_INET, SOCK_STREAM, 0);
    dataSocket.FD = socket(AF_INET, SOCK_STREAM, 0);
    maxFD = max(requestSocket.FD, dataSocket.FD);
    
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

    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);
    FD_SET(requestSocket.FD, &readSet);
    FD_SET(dataSocket.FD, &readSet);
}

void ServerAPI::AcceptClient() {
    struct sockaddr_in clientAddress;
    int addressLen = sizeof(clientAddress);
    int newClientFD = accept(requestSocket.FD, (struct sockaddr *)&clientAddress, (socklen_t *)&addressLen);
    maxFD = max(maxFD, newClientFD);
    Client newClient;
    newClient.dataFD = -1;
    newClient.message = Utility::ToStr(newClientFD);
    newClient.isDownloading = false;
    clients[newClientFD] = newClient;
    FD_SET(newClientFD, &writeSet);
    logger->Log("New client was connected to server.");
}

void ServerAPI::AcceptDataClient() {
    struct sockaddr_in clientAddress;
    int addressLen = sizeof(clientAddress);
    int newClientFD = accept(dataSocket.FD, (struct sockaddr *)&clientAddress, (socklen_t *)&addressLen);
    maxFD = max(maxFD, newClientFD);
    unmappedClients.insert(newClientFD);
    FD_SET(newClientFD, &readSet);
}

void ServerAPI::GetRequest(int clientFD) {
    memset(buf, 0, MAX_BUF_SIZE + 1);
    if (recv(clientFD, buf, MAX_BUF_SIZE, 0) == 0) {
        serverCore->Quit(clientFD);
        close(clientFD);
        clients.erase(clientFD);
        FD_CLR(clientFD, &readSet);
        return;
    }
    string request(buf);
    string response = HandleRequest(request, clientFD);
    clients[clientFD].message = response;
    FD_CLR(clientFD, &readSet);
    FD_SET(clientFD, &writeSet);
}

void ServerAPI::SendMessage(int clientFD) {
    string message = clients[clientFD].message;
    if (send(clientFD, message.c_str(), strlen(message.c_str()), 0) == -1) {
        serverCore->Quit(clientFD);
        close(clientFD);
        clients.erase(clientFD);
        FD_CLR(clientFD, &writeSet);
        return;
    }
    FD_CLR(clientFD, &writeSet);
    Client client = clients[clientFD];
    if (client.isDownloading)
        FD_SET(client.dataFD, &writeSet);
    else
        FD_SET(clientFD, &readSet);

}

void ServerAPI::MapDataClient(int clientDataFD)
{
    memset(buf, 0, MAX_BUF_SIZE + 1);
    if (recv(clientDataFD, buf, MAX_BUF_SIZE, 0) == 0) {
        close(clientDataFD);
        FD_CLR(clientDataFD, &readSet);
        return;
    }
    int clientFD = atoi(buf);
    if (clients.find(clientFD) != clients.end()) {
        clients[clientFD].dataFD = clientDataFD;
        unmappedClients.erase(clientDataFD);
        FD_CLR(clientDataFD, &readSet);
        FD_SET(clientFD, &readSet);
    }
}

void ServerAPI::StartDownload(int clientFD) {
    memset(buf, 0, MAX_BUF_SIZE + 1);
    int dataFD = clients[clientFD].dataFD;
    if (recv(dataFD, buf, MAX_BUF_SIZE, 0) == 0) {
        close(dataFD);
        FD_CLR(dataFD, &readSet);
        return;
    }
    FD_CLR(dataFD, &readSet);
    FD_SET(dataFD, &writeSet);
}

void ServerAPI::SendData(int clientFD) {
    Client client = clients[clientFD];
    if (client.isDownloading) {
        string len = Utility::ToStr(client.fileLen);
        if (send(client.dataFD, len.c_str(), strlen(len.c_str()), 0) == -1) {
            close(client.dataFD);
            return;
        }
        clients[clientFD].isDownloading = false;
        FD_CLR(client.dataFD, &writeSet);
        FD_SET(client.dataFD, &readSet);
    }
    else {
        if (send(client.dataFD, client.contentParts[0].c_str(), strlen(client.contentParts[0].c_str()), 0) == -1) {
            close(clientFD);
            return;
        }
        clients[clientFD].contentParts.erase(clients[clientFD].contentParts.begin());
        if (clients[clientFD].contentParts.size() == 0) {
            FD_CLR(client.dataFD, &writeSet);
            FD_SET(clientFD, &readSet);
        }
    }
}

void ServerAPI::HandleRequests() {
    if (FD_ISSET(requestSocket.FD, &workingReadSet))
        AcceptClient();
    vector c(clients.begin(), clients.end());
    for (auto client : c) {
        if (FD_ISSET(client.first, &workingReadSet)) 
            GetRequest(client.first);
        if (FD_ISSET(client.first, &workingWriteSet))
            SendMessage(client.first);
    }
}

void ServerAPI::HandleDownloads() {
    if (FD_ISSET(dataSocket.FD, &workingReadSet))
        AcceptDataClient();
    
    vector c(clients.begin(), clients.end());
    for (auto client : c) {
        int dataFD = client.second.dataFD;
        if (dataFD < 0) continue;
        if (FD_ISSET(dataFD, &workingReadSet))
            StartDownload(client.first);
        if (FD_ISSET(dataFD, &workingWriteSet))
            SendData(client.first);
    }

    vector uc(unmappedClients.begin(), unmappedClients.end());
    for (int clientFD : uc) {
        if (FD_ISSET(clientFD, &workingReadSet)) 
            MapDataClient(clientFD);
    }
}

void ServerAPI::Run() {
    logger->Log("Initializing server.");
    SetupSockets();
    StartListening();
    logger->Log("Started listening for clients");
    while (true) {
        workingReadSet = readSet;
        workingWriteSet = writeSet;
        select(maxFD + 1, &workingReadSet, &workingWriteSet, NULL, NULL);
        HandleRequests();
        HandleDownloads();
    }
}