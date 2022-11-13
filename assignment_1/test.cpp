#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <nlohmann/json.hpp>

int main(){
    std::string teststr = "";
    teststr += "(" + std::to_string(15) + "," + std::to_string(99) + ")";
    std::vector<std::pair<int,int>> vec = {std::pair<int,int>(12,55),std::pair<int,int>(15,22)};
    
    nlohmann::json asdf;
    nlohmann::json asdf2;
    asdf[teststr] = nlohmann::json({std::to_string(1337), vec});
    std::ofstream o("cycles_16k.ndjson");
    o << asdf << std::endl;
    return EXIT_SUCCESS;
}
