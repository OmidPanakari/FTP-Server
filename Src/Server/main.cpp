#include "Logger.hpp"
#include "ServerCore.hpp"
#include "JsonSerializer.hpp"
#include "ServerAPI.hpp"
#include <iostream>
#include <vector>

using namespace std;
int main() {
    ServerAPI server;
    server.Run();
    return 0;
}