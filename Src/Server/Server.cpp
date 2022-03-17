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
    cout << sc.Authenticate("1234", 1) << endl;
    auto resp = sc.GetCurrentDirectory(1);
    cout << resp.code << ' ' << resp.directory << endl;
    cout << sc.MakeDirectory("../../amir/../ehsan", 1) << endl;
    return 0;
}