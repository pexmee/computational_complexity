// #include "../headers/traverser.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <vector>
#include <cmath>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <nlohmann/json.hpp>
#include <fstream>
#include <time.h>
#include <random>
#include <ctime>


#define DATA16K "../data16k.txt"
#define INT_MAX 2147483647
#define DATA128 "../data128.txt"
#define POPULATION 20
#define MUTATION_RATE 0.3
auto def_gen = std::default_random_engine{};
std::mt19937 gen(std::random_device{}());

std::vector<double> calc_probabilities(std::vector<double> fitness_scores){
    double total_fitness = 0;
    std::vector<double> probabilities = {};

    for(size_t i = 0; i < fitness_scores.size(); i++){
        total_fitness += fitness_scores[i];
    }
    for(size_t i = 0; i < fitness_scores.size(); i++){
        probabilities.push_back(fitness_scores[i] / total_fitness);
    }
    return probabilities;
}

__always_inline double calc_dist(std::vector<int> &a, std::vector<int> &b){
    return std::sqrt(std::pow((a[0] - b[0]),2) + std::pow((a[1] - b[1]),2));

}

std::vector<std::vector<std::vector<int>>> genesis(std::vector<std::vector<int>> nodes){
    std::vector<std::vector<std::vector<int>>> population_set = {};
    std::vector<std::vector<int>> solution = {};
    std::vector<std::vector<int>> tmp = nodes;

    for(size_t i = 0; i < POPULATION; i++){

        solution = {};
        nodes = tmp;
        std::shuffle(std::begin(tmp), std::end(tmp), def_gen);
        solution = tmp;
        tmp = nodes;
            
        population_set.push_back(solution);
    }
    return population_set;
}

double fitness_eval(std::vector<std::vector<int>> city_list){
    double total = 0;
    std::vector<int> a, b;
    for (size_t i = 0; i < city_list.size()-1; i++){
        a = city_list[i];
        b = city_list[i+1];
        total += calc_dist(a,b);
    }
    return total;
}

std::vector<double> get_all_fitness(std::vector<std::vector<std::vector<int>>> population_set){
    std::vector<double> fitness_list;

    for(unsigned i = 0; i < population_set.size(); i++){
        fitness_list.push_back(fitness_eval(population_set[i]));
    }

    return fitness_list;
}

double sum_total_fit(std::vector<double> fitness_list){
    double total = 0;

    for (size_t i = 0; i < fitness_list.size(); i++){
        total += fitness_list[i];
    }
    return total;
}

std::vector<std::vector<std::vector<std::vector<int>>>> progenitor_selection(std::vector<std::vector<std::vector<int>>> population_set, std::vector<double> fitness_list){
    std::vector<std::vector<std::vector<int>>> parents_a, parents_b;
    std::vector<double> probabilities = calc_probabilities(fitness_list);
    std::discrete_distribution<std::size_t> d{probabilities.begin(), probabilities.end()};

    for(size_t i = 0; i < probabilities.size(); i++){
        parents_a.push_back(population_set[d(gen)]);
        parents_b.push_back(population_set[d(gen)]);
    }
    std::vector<std::vector<std::vector<std::vector<int>>>> ret = {parents_a, parents_b};
    return ret;
}

std::vector<std::vector<int>> mate_progenitors(std::vector<std::vector<int>> prog_a, std::vector<std::vector<int>> prog_b){
    std::vector<std::vector<int>> offspring;
    for(size_t i = 0; i < prog_a.size()/3; i++){
        offspring.push_back(prog_a[i]);
    }

    for(size_t i = 0; i < prog_b.size(); i++){
        if (std::find(offspring.begin(), offspring.end(), prog_b[i]) == offspring.end()){
            offspring.push_back(prog_b[i]);
        }
    }

    return offspring;
}

std::vector<std::vector<std::vector<int>>> mate_population(std::vector<std::vector<std::vector<std::vector<int>>>> progenitor_list){
    std::vector<std::vector<std::vector<int>>> new_population_set;
    
    for (size_t i = 0; i < progenitor_list[0].size(); i++){
        std::vector<std::vector<int>> prog_a = progenitor_list[0][i];
        std::vector<std::vector<int>> prog_b = progenitor_list[1][i];
        std::vector<std::vector<int>> offspring = mate_progenitors(prog_a, prog_b);
        new_population_set.push_back(offspring);
    }

    return new_population_set;
}

std::vector<std::vector<int>> mutate_offspring(std::vector<std::vector<int>> offspring){
    int a,b;
    for (size_t i = 0; i < offspring.size()*MUTATION_RATE; i++){
        a = rand()%(offspring.size());
        b = rand()%(offspring.size());
        std::vector<int> tmp_a = offspring[a];
        std::vector<int> tmp_b = offspring[b];
        offspring[a] = tmp_b;
        offspring[b] = tmp_a;
    }
    return offspring;
}

std::vector<std::vector<std::vector<int>>> mutate_population(std::vector<std::vector<std::vector<int>>> new_population_set){
    std::vector<std::vector<std::vector<int>>> mutated_pop;
    for(size_t i = 0; i < new_population_set.size(); i++){
        mutated_pop.push_back(mutate_offspring(new_population_set[i]));
    }
    return mutated_pop;
}

double get_smallest_fitness(std::vector<double> fitness_list){
    double smallest = INT_MAX;
    for (size_t i = 0; i < fitness_list.size(); i++){
        if (fitness_list[i] < smallest){
            smallest = fitness_list[i];
        }
    }
    return smallest;
}

int main(){
    time_t now;
    std::ifstream data_file(DATA16K);
    std::string line;
    std::vector<std::vector<int>> nodes;
    while(getline(data_file,line)){
        int x, y;
        std::stringstream ss(line);
        ss >> x;
        ss >> y;
        std::vector<int> node = {x,y};
        if (std::find(nodes.begin(), nodes.end(), node) == nodes.end()){ // remove duplicates.
            nodes.push_back(node);
        }
    }
    std::cout << "nodes: " << nodes.size() << std::endl;

    std::vector<std::vector<std::vector<int>>> population_set = genesis(nodes);
    std::vector<double> fitness_list = get_all_fitness(population_set);
    std::vector<std::vector<std::vector<std::vector<int>>>> progenitor_list = progenitor_selection(population_set, fitness_list);
    std::vector<std::vector<std::vector<int>>> new_population_set = mate_population(progenitor_list);
    std::vector<std::vector<std::vector<int>>> mutated_pop = mutate_population(new_population_set);
    double best_solution = INT_MAX;

    while (true){
        fitness_list = get_all_fitness(mutated_pop);
        double smallest_current = get_smallest_fitness(fitness_list);
        if (smallest_current < best_solution){
            best_solution = smallest_current;
            now = time(nullptr);
            char* dt = ctime(&now);
            std::cout << dt << " found new best solution: " << std::fixed << best_solution << std::endl;
        }
        progenitor_list = progenitor_selection(population_set, fitness_list);
        new_population_set = mate_population(progenitor_list);
        mutated_pop = mutate_population(new_population_set);
    }
}