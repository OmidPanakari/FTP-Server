#include "Logger.hpp"
#include "ServerCore.hpp"
#include "JsonSerializer.hpp"
#include <iostream>
#include <vector>

using namespace std;
int main(){
    JsonSerializer js;
    js.AddItem("Omid", "Kir Nadarad");
    vector<string> s = {"Omid", "Kir", "Nadarad"};
    js.AddList("Mammad", s);
    JsonSerializer jss;
    jss.ReadJson(js.GetJson());
    cout << js.GetStrArray("Mammad")[0];


    return 0;
}