#include "Client.hpp"
#include "Utility.hpp"
#include <iostream>
#include <fstream>
#include <sys/stat.h>

using namespace std;

Client::Client() {
    JsonSerializer config;
    config.ReadFile(CONFIGFILE);
    requestPort = config.GetInteger("requestPort");
    dataPort = config.GetInteger("dataPort");
    struct stat buffer;
    if (stat(DOWNLOAD_DIR, &buffer) != 0) {
        mkdir(DOWNLOAD_DIR, 0777);
    }
}

void Client::ConnectServer() {
    cout << "Connecting to server..." << endl;
    struct sockaddr_in requestAddress, dataAddress;
    requestFD = socket(AF_INET, SOCK_STREAM, 0);
    dataFD = socket(AF_INET, SOCK_STREAM, 0);

    requestAddress.sin_family = AF_INET;
    requestAddress.sin_port = htons(requestPort);
    requestAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    dataAddress = requestAddress;
    dataAddress.sin_port = htons(dataPort);

    if (connect(requestFD, (struct sockaddr *)&requestAddress, sizeof(requestAddress)) < 0) {
        cout << "Cannot connect to request socket!" << endl;
    }

    if (connect(dataFD, (struct sockaddr *)&dataAddress, sizeof(dataAddress)) < 0) {
        cout << "Cannot connect to data socket!" << endl;
    }

    memset(buf, 0, MAX_BUF_SIZE + 1);
    recv(requestFD, buf, MAX_BUF_SIZE, 0);
    send(dataFD, buf, strlen(buf), 0);

    cout << "Welcome to FTP-Server :)" << endl;
}

string Client::Download(int dataSize) {
    string content = "";
    while(strlen(content.c_str()) != (uint)dataSize) {
        memset(buf, 0, MAX_BUF_SIZE + 1);
        recv(dataFD, buf, MAX_BUF_SIZE, 0);
        content += buf;
    }
    return content;
}

void Client::SaveFile(string path, string content)  {
    ofstream file(path);
    file << content;
    file.close();
}

void Client::HandleCommand(string command) {
    vector<string> commandVector = Utility::Split(command, ' ');
    if (commandVector.size() < 1) {
        cout << SYNTAX_ERROR_MESSAGE << endl;
        return;
    }
    if (commandVector[0] == USER) {
        commandVector.erase(commandVector.begin());
        CheckUsername(commandVector);
    } else if (commandVector[0] == PASS) {
        commandVector.erase(commandVector.begin());
        Authenticate(commandVector);
    } else if (commandVector[0] == PWD) {
        commandVector.erase(commandVector.begin());
        GetCurrentDirectory(commandVector);
    } else if (commandVector[0] == MKD) {
        commandVector.erase(commandVector.begin());
        MakeDirectory(commandVector);
    } else if(commandVector[0] == DELE) {
        commandVector.erase(commandVector.begin());
        DeleteFileOrDirectory(commandVector);
    } else if(commandVector[0] == LS) {
        commandVector.erase(commandVector.begin());
        ShowList(commandVector);
    } else if (commandVector[0] == CWD) {
        commandVector.erase(commandVector.begin());
        ChangeDirectory(commandVector);
    } else if(commandVector[0] == RENAME) {
        commandVector.erase(commandVector.begin());
        RenameFile(commandVector);
    } else if (commandVector[0] == RETR) {
        commandVector.erase(commandVector.begin());
        GetFile(commandVector);
    } else if (commandVector[0] == HELP) {
        commandVector.erase(commandVector.begin());
        Help(commandVector);
    } else if(commandVector[0] == QUIT) {
        commandVector.erase(commandVector.begin());
        Quit(commandVector);
    } else 
        cout << SYNTAX_ERROR_MESSAGE << endl;
}

void Client::CheckUsername(vector <string> command) {
    if (command.size() != 1) {
        cout << SYNTAX_ERROR_MESSAGE << endl;
        return;
    }

    JsonSerializer requestSerializer;
    requestSerializer.AddItem("method", USER);
    requestSerializer.AddItem("username", command[0]);
    string request = requestSerializer.GetJson();
    send(requestFD, request.c_str(), strlen(request.c_str()), 0);
    memset(buf, 0, MAX_BUF_SIZE + 1);
    recv(requestFD, buf, MAX_BUF_SIZE, 0);
    string response(buf);
    JsonSerializer responseDeserializer;
    responseDeserializer.ReadJson(response);

    switch (responseDeserializer.GetInteger("code")) {
        case 425:
            cout << MAXIMUM_USERS_ERROR_MESSAGE << endl;
            break;
        case 331:
            cout << USER_OK << endl;
            break;
        case 430:
            cout << INVALID_CREDENTIALS_ERROR_MESSAGE << endl;
            break;
        default:
            cout << UNKNOWN_ERROR_MESSAGE << endl;
    }
}

void Client::Authenticate(vector <string> command) {
    if (command.size() != 1) {
        cout << SYNTAX_ERROR_MESSAGE << endl;
        return;
    }

    JsonSerializer requestSerializer;
    requestSerializer.AddItem("method", PASS);
    requestSerializer.AddItem("password", command[0]);
    string request = requestSerializer.GetJson();
    send(requestFD, request.c_str(), strlen(request.c_str()), 0);
    memset(buf, 0, MAX_BUF_SIZE + 1);
    recv(requestFD, buf, MAX_BUF_SIZE, 0);
    string response(buf);
    JsonSerializer responseDeserializer;
    responseDeserializer.ReadJson(response);

    switch (responseDeserializer.GetInteger("code")) {
        case 230:
            cout << LOGIN_OK << endl;
            break;
        case 503:
            cout << BAD_SEQUENCE_ERROR_MESSAGE << endl;
            break;
        case 430:
            cout << INVALID_CREDENTIALS_ERROR_MESSAGE << endl;
            break;
        default:
            cout << UNKNOWN_ERROR_MESSAGE << endl;
    }
}

void Client::GetCurrentDirectory(vector <string> command) {
    if (command.size() != 0) {
        cout << SYNTAX_ERROR_MESSAGE << endl;
        return;
    }

    JsonSerializer requestSerializer;
    requestSerializer.AddItem("method", PWD);
    string request = requestSerializer.GetJson();
    send(requestFD, request.c_str(), strlen(request.c_str()), 0);
    memset(buf, 0, MAX_BUF_SIZE + 1);
    recv(requestFD, buf, MAX_BUF_SIZE, 0);
    string response(buf);
    JsonSerializer responseDeserializer;
    responseDeserializer.ReadJson(response);

    switch (responseDeserializer.GetInteger("code")) {
        case 332:
            cout << UNAUTHORIZED_ERROR_MESSAGE << endl;
            break;
        case 257:
            cout << "257: " + responseDeserializer.Get("directory") << endl;
            break;
        default:
            cout << UNKNOWN_ERROR_MESSAGE << endl;
    }
}

void Client::MakeDirectory(vector <string> command) {
    if (command.size() != 1) {
        cout << SYNTAX_ERROR_MESSAGE << endl;
        return;
    }

    JsonSerializer requestSerializer;
    requestSerializer.AddItem("method", MKD);
    requestSerializer.AddItem("path", command[0]);
    string request = requestSerializer.GetJson();
    send(requestFD, request.c_str(), strlen(request.c_str()), 0);
    memset(buf, 0, MAX_BUF_SIZE + 1);
    recv(requestFD, buf, MAX_BUF_SIZE, 0);
    string response(buf);
    JsonSerializer responseDeserializer;
    responseDeserializer.ReadJson(response);

    switch (responseDeserializer.GetInteger("code")) {
        case 332:
            cout << UNAUTHORIZED_ERROR_MESSAGE << endl;
            break;
        case 257:
            cout << "257: " + command[0] + " created." << endl;
            break;
        default:
           cout << UNKNOWN_ERROR_MESSAGE << endl;
    }
}

void Client::DeleteFileOrDirectory(vector <string> command) {
    if (command.size() != 2 || (command[0] != "-d" && command[0] != "-f")) {
        cout << SYNTAX_ERROR_MESSAGE << endl;
        return;
    }

    JsonSerializer requestSerializer;
    requestSerializer.AddItem("method", DELE);
    requestSerializer.AddItem("path", command[1]);
    string request = requestSerializer.GetJson();
    send(requestFD, request.c_str(), strlen(request.c_str()), 0);
    memset(buf, 0, MAX_BUF_SIZE + 1);
    recv(requestFD, buf, MAX_BUF_SIZE, 0);
    string response(buf);
    JsonSerializer responseDeserializer;
    responseDeserializer.ReadJson(response);

    switch (responseDeserializer.GetInteger("code")) {
        case 332:
            cout << UNAUTHORIZED_ERROR_MESSAGE << endl;
            break;
        case 250:
           cout << "250: " + command[1] + " deleted." << endl;
            break;
        case 550:
            cout << PERMISSION_DENIED_ERROR_MESSAGE << endl;
            break;
        default:
            cout << UNKNOWN_ERROR_MESSAGE << endl;
    }
}

void Client::ShowList(vector <string> command) {
    if (command.size() != 0) {
        cout << SYNTAX_ERROR_MESSAGE << endl;
        return;
    }

    JsonSerializer requestSerializer;
    requestSerializer.AddItem("method", LS);
    string request = requestSerializer.GetJson();
    send(requestFD, request.c_str(), strlen(request.c_str()), 0);
    memset(buf, 0, MAX_BUF_SIZE + 1);
    recv(requestFD, buf, MAX_BUF_SIZE, 0);
    string response(buf);
    JsonSerializer responseDeserializer;
    responseDeserializer.ReadJson(response);

    string content;
    switch (responseDeserializer.GetInteger("code")) {
        case 332:
            cout << UNAUTHORIZED_ERROR_MESSAGE << endl;
            break;
        case 226:
            content = Download(responseDeserializer.GetInteger("size"));
            cout << content;
            cout << LIST_TRANSFER_OK << endl;
            break;
        default:
            cout << UNKNOWN_ERROR_MESSAGE << endl;
    }
}

void Client::ChangeDirectory(vector <string> command) {
    if (command.size() != 1) {
        cout << SYNTAX_ERROR_MESSAGE << endl;
        return;
    }

    JsonSerializer requestSerializer;
    requestSerializer.AddItem("method", CWD);
    requestSerializer.AddItem("path", command[0]);
    string request = requestSerializer.GetJson();
    send(requestFD, request.c_str(), strlen(request.c_str()), 0);
    memset(buf, 0, MAX_BUF_SIZE + 1);
    recv(requestFD, buf, MAX_BUF_SIZE, 0);
    string response(buf);
    JsonSerializer responseDeserializer;
    responseDeserializer.ReadJson(response);

    switch (responseDeserializer.GetInteger("code")) {
        case 332:
            cout << UNAUTHORIZED_ERROR_MESSAGE << endl;
            break;
        case 250:
            cout << CHANGE_DIRECTORY_OK << endl;
            break;
        default:
            cout << UNKNOWN_ERROR_MESSAGE << endl;
    }
}

void Client::RenameFile(vector <string> command) {
    if (command.size() != 2) {
        cout << SYNTAX_ERROR_MESSAGE << endl;
        return;
    }

    JsonSerializer requestSerializer;
    requestSerializer.AddItem("method", RENAME);
    requestSerializer.AddItem("path", command[0]);
    requestSerializer.AddItem("newPath", command[1]);
    string request = requestSerializer.GetJson();
    send(requestFD, request.c_str(), strlen(request.c_str()), 0);
    memset(buf, 0, MAX_BUF_SIZE + 1);
    recv(requestFD, buf, MAX_BUF_SIZE, 0);
    string response(buf);
    JsonSerializer responseDeserializer;
    responseDeserializer.ReadJson(response);

    switch (responseDeserializer.GetInteger("code")) {
        case 332:
            cout << UNAUTHORIZED_ERROR_MESSAGE << endl;
            break;
        case 250:
            cout << CHANGE_NAME_OK << endl;
            break;
        case 550:
            cout << PERMISSION_DENIED_ERROR_MESSAGE << endl;
            break;
        default:
            cout << UNKNOWN_ERROR_MESSAGE << endl;
    }
}

void Client::GetFile(vector <string> command) {
    if (command.size() != 1) {
        cout << SYNTAX_ERROR_MESSAGE << endl;
        return;
    }

    JsonSerializer requestSerializer;
    requestSerializer.AddItem("method", RETR);
    requestSerializer.AddItem("filename", command[0]);
    string request = requestSerializer.GetJson();
    send(requestFD, request.c_str(), strlen(request.c_str()), 0);
    memset(buf, 0, MAX_BUF_SIZE + 1);
    recv(requestFD, buf, MAX_BUF_SIZE, 0);
    string response(buf);
    JsonSerializer responseDeserializer;
    responseDeserializer.ReadJson(response);

    string content, path;
    switch (responseDeserializer.GetInteger("code")) {
        case 425:
            cout << FILE_SIZE_ERROR_MESSAGE << endl;
            break;
        case 332:
            cout << UNAUTHORIZED_ERROR_MESSAGE << endl;
            break;
        case 226:
            content = Download(responseDeserializer.GetInteger("size"));
            path = string(DOWNLOAD_DIR) + "/" + command[0];
            SaveFile(path, content);
            cout << DOWNLOAD_OK << endl;
            break;
        default:
            cout << UNKNOWN_ERROR_MESSAGE << endl;
    }
}

void Client::Help(vector <string> command) {
    if (command.size() != 0) {
        cout << SYNTAX_ERROR_MESSAGE << endl;
        return;
    }

    cout << 214 << endl;
    cout << "USER [name], Its argument is used to specify the user's username. It is used for user authentication." << endl;
    cout << "PASS [password], Its argument is used to specify the user's password. It is used for user authentication." << endl;
    cout << "PWD, It is used for displaying user's current working directory." << endl;
    cout << "MKD [path], Its argument is used to specify the new directory. It is used for making new directory." << endl;
    cout << "DELE -f [filePath], Its argument is used to specify the file path. It is used for deleting a file." << endl;
    cout << "DELE -d [directoryPath], Its argument is used to specify the directory path. It is used for deleting a directory." << endl;
    cout << "LS, It is used for displaying list of files and directories in current working directory." << endl;
    cout << "CWD [path], Its argument is used to specify new directory. It is used for changing current working directory to the new directory." << endl;
    cout << "RENAME [path] [newName], Its first argument specifies the path for target file and Its second argument is the new name for this file. It is used for changing name of file." << endl;
    cout << "RETR [name], Its argument is used to specify file name. It is used for downloading file." << endl;
    cout << "QUIT, It is used for quiting from system." << endl;
    cout << "HELP, It is used for displaying list of all available commands." << endl;
}

void Client::Quit(vector <string> command) {
    if (command.size() != 0) {
        cout << SYNTAX_ERROR_MESSAGE << endl;
        return;
    }

    JsonSerializer requestSerializer;
    requestSerializer.AddItem("method", QUIT);
    string request = requestSerializer.GetJson();
    send(requestFD, request.c_str(), strlen(request.c_str()), 0);
    memset(buf, 0, MAX_BUF_SIZE + 1);
    recv(requestFD, buf, MAX_BUF_SIZE, 0);
    string response(buf);
    JsonSerializer responseDeserializer;
    responseDeserializer.ReadJson(response);

    switch (responseDeserializer.GetInteger("code")) {
        case 332:
            cout << UNAUTHORIZED_ERROR_MESSAGE << endl;
            break;
        case 221:
            cout << QUIT_OK << endl;
            break;
        default:
            cout << UNKNOWN_ERROR_MESSAGE << endl;
    }
}


void Client::Run() {
    ConnectServer();
    string command;
    while (true) {
        getline(cin, command);
        HandleCommand(command);
    }
}