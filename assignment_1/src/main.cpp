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
#include "../headers/node.hpp"


#define DATA16K "../data16k.txt"
#define INT_MAX 2147483647
#define DATA128 "../data128.txt"
#define CYCLE_FILE_16K "cycles_16k.ndjson"
#define CYCLE_FILE_128 "cycles_128.ndjson"
#define POPULATION 20
#define MUTATION_RATE 0.3

using namespace std;
auto def_gen = default_random_engine{};
mt19937 gen(random_device{}());

vector<double> calc_probabilities(const vector<double> &fitness_scores){
    double total_fitness = 0;
    vector<double> probabilities = {};

    for(size_t i = 0; i < fitness_scores.size(); i++){
        total_fitness += fitness_scores[i];
    }
    for(size_t i = 0; i < fitness_scores.size(); i++){
        probabilities.push_back(fitness_scores[i] / total_fitness);
    }
    return probabilities;
}

__always_inline double calc_dist(Node &a, Node &b){
    return sqrt(pow((a.x - b.x),2) + pow((a.y - b.y),2));

}

vector<vector<Node>> genesis(vector<Node> nodes){
    vector<vector<Node>> population_set = {};
    vector<Node> solution = nodes;

    for(size_t i = 0; i < POPULATION; i++){
        solution = nodes;
        shuffle(begin(solution), end(solution), def_gen);
        population_set.push_back(solution);
    }
    return population_set;
}

double fitness_eval(const vector<Node> &city_list){
    double total = 0;
    Node a, b;
    for (size_t i = 0; i < city_list.size()-1; i++){
        a = city_list[i];
        b = city_list[i+1];
        total += calc_dist(a,b);
    }
    return total;
}

vector<double> get_all_fitness(const vector<vector<Node>> &population_set){
    vector<double> fitness_list;

    for(unsigned i = 0; i < population_set.size(); i++){
        fitness_list.push_back(fitness_eval(population_set[i]));
    }

    return fitness_list;
}

double sum_total_fit(const vector<double> &fitness_list){
    double total = 0;

    for (size_t i = 0; i < fitness_list.size(); i++){
        total += fitness_list[i];
    }
    return total;
}

vector<vector<vector<Node>>> progenitor_selection(const vector<vector<Node>> &population_set, const vector<double> &fitness_list){
    vector<vector<Node>> parents_a, parents_b;
    vector<double> probabilities = calc_probabilities(fitness_list);
    discrete_distribution<size_t> d{probabilities.begin(), probabilities.end()};

    for(size_t i = 0; i < probabilities.size(); i++){
        parents_a.push_back(population_set[d(gen)]);
        parents_b.push_back(population_set[d(gen)]);
    }
    vector<vector<vector<Node>>> ret = {parents_a, parents_b};
    return ret;
}

vector<Node> mate_progenitors(const vector<Node> &prog_a, const vector<Node> &prog_b){
    vector<Node> offspring;
    for(size_t i = 0; i < prog_a.size()/3; i++){
        offspring.push_back(prog_a[i]);
    }

    for(size_t i = 0; i < prog_b.size(); i++){
        if (find(offspring.begin(), offspring.end(), prog_b[i]) == offspring.end()){
            offspring.push_back(prog_b[i]);
        }
    }

    return offspring;
}

vector<vector<Node>> mate_population(const vector<vector<vector<Node>>> &progenitor_list){
    vector<vector<Node>> new_population_set;
    
    for (size_t i = 0; i < progenitor_list[0].size(); i++){
        vector<Node> prog_a = progenitor_list[0][i];
        vector<Node> prog_b = progenitor_list[1][i];
        vector<Node> offspring = mate_progenitors(prog_a, prog_b);
        new_population_set.push_back(offspring);
    }

    return new_population_set;
}

void mutate_offspring(vector<Node> &offspring){
    int a,b;
    for (size_t i = 0; i < offspring.size()*MUTATION_RATE; i++){
        a = rand()%(offspring.size());
        b = rand()%(offspring.size());
        Node tmp_a = offspring[a];
        Node tmp_b = offspring[b];
        offspring[a] = tmp_b;
        offspring[b] = tmp_a;
    }
}

void mutate_population(vector<vector<Node>> &new_population_set){
    for(size_t i = 0; i < new_population_set.size(); i++){
        mutate_offspring(new_population_set[i]);
    }
}

pair<double,int> get_best_fitness(const vector<double> &fitness_list){
    pair<double,int> best_and_index = {INT_MAX, -1};
    for (size_t i = 0; i < fitness_list.size(); i++){
        if (fitness_list[i] < best_and_index.first){
            best_and_index.first = fitness_list[i];
            best_and_index.second = i;
        }
    }
    return best_and_index;
}

// For serializing the cycle to json object
__always_inline void to_json(nlohmann::json &j, const Node &node) {
    j = node.serialize();
}

void dump_cycle(pair<double,int> &best_solution, const vector<vector<Node>> &pop, ofstream &o){
    time_t now;
    now = time(nullptr);
    char* dt = ctime(&now);
    cout << dt << " found new best solution: " << fixed << best_solution.first << endl;
    string start_coord = "(" + to_string(pop[best_solution.second][0].x) + "," + to_string(pop[best_solution.second][0].y) + ")";
    nlohmann::json data;
    data["dist"] = best_solution.first;
    data["start_coord"] = start_coord;
    data["timestamp"] = dt;
    data["zcycle"] = pop[best_solution.second]; // zcycle to order it to the back of the ndjson
    o << data << endl;
    
}


int main(){
    ofstream o;
    ifstream data_file(DATA128);
    o.open(CYCLE_FILE_128, ios_base::app);
    string line;
    vector<Node> nodes;

    while(getline(data_file,line)){
        int x, y;
        stringstream ss(line);
        ss >> x;
        ss >> y;
        Node node(x,y);
        if (find(nodes.begin(), nodes.end(), node) == nodes.end()){ // remove duplicates.
            nodes.push_back(node);
        }
    }
    cout << "nodes: " << nodes.size() << endl;
    Node **arr = new Node*[nodes.size()];

    vector<vector<Node>> population_set = genesis(nodes);
    vector<double> fitness_list = get_all_fitness(population_set);
    vector<vector<vector<Node>>> progenitor_list = progenitor_selection(population_set, fitness_list);
    vector<vector<Node>> new_population_set = mate_population(progenitor_list);
    mutate_population(new_population_set);
    pair<double,int> best_solution = {INT_MAX,-1};
    pair<double,int> smallest_current = {INT_MAX,-1};

    while (true){
        fitness_list = get_all_fitness(new_population_set);
        smallest_current = get_best_fitness(fitness_list);
        if (smallest_current.first < best_solution.first){
            best_solution.first = smallest_current.first;
            best_solution.second = smallest_current.second;
            dump_cycle(best_solution, new_population_set, o);
        }
        progenitor_list = progenitor_selection(population_set, fitness_list);
        new_population_set = mate_population(progenitor_list);
        mutate_population(new_population_set);
    }
}