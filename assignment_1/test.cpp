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
    std::mt19937 gen(std::random_device{}());

    std::vector<double> probabilities = {0.1,0.1, 0.2,0.2,0.4};
    std::vector<std::string> tmp = {"adam", "nisse", "bertil", "peter","rick"};
    std::discrete_distribution<std::size_t> d{probabilities.begin(), probabilities.end()};
    auto sample = tmp[d(gen)];
    std::cout << "sample: " << sample << std::endl;
}
