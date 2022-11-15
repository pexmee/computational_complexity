#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <nlohmann/json.hpp>
#include <random>

int main(){
    int size = 10;
    // std::mt19937 gen(std::random_device{}());

    // std::vector<double> probabilities = {0.1,0.1, 0.2,0.2,0.4};
    // std::vector<std::string> tmp = {"adam", "nisse", "bertil", "peter","rick"};
    // std::discrete_distribution<std::size_t> d{probabilities.begin(), probabilities.end()};
    // auto sample = tmp[d(gen)];
    // std::cout << "sample: " << sample << std::endl;
    std::vector<int> test = {10,20,30,40,50,60,70,80,90,100};
    for(unsigned i = 0; i < test.size()-1; i++){
        std::cout << "i: " << i << " i+1: " << i+1 << std::endl;
    }
}
