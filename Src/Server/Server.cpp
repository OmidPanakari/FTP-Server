#include "Logger.hpp"
#include "ServerCore.hpp"
#include "JsonSerializer.hpp"
#include <iostream>

using namespace std;
int main(){
    JsonSerializer js;
    js.ReadFile("config.json");
    ServerCore sc(js);
    cout << sc.CheckUsername("Amir", 1) << endl;
    cout << sc.Authenticate("1234", 2) << endl;

    return 0;
}