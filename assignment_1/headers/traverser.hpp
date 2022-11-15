#ifndef TRAVERSER_HPP
#define TRAVERSER_HPP

/* Includes */
#include "node.hpp"
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

/* Defines */
#define DATA128 "data128.txt"
#define DATA16K "data16k.txt"
#define CYCLE_FILE "cycles_16k.ndjson"
#define MAX_X
#define MAX_Y
#define INT_MAX 2147483647
#define CLUSTER_SIZE 500
#define MUTATION_RATE 0.3
#define ITERATIONS 1000000
#define POPULATION_SIZE 100

class Traverser{
    
    private:
        std::vector<Node> nodes;
        std::vector<Node> cycle;
        std::vector<std::vector<Node>> parent_population;
        std::vector<std::vector<Node>> new_generation;
        std::vector<Node> possible_starting_nodes;
        std::vector<double> fitness_scores;
        Node* current_node;
        double distance_traversed;
        bool starting_nodes_set;
        
    public:
        Traverser();
        ~Traverser();
        void graph();
        void initialize_nodes();
        void close_cycle();
        double calc_dist(Node &node);
        void dump_cycle(double fitness, std::vector<Node> &cycle);
        void mutate();
        void copulate(std::vector<std::vector<std::vector<Node>>> &parents);
        void randomize_parents();
        std::vector<std::vector<std::vector<Node>>> select_parents(); // This is obviously stupid but I shall fix this later on.
        void get_data(); // Need to be modified a bit from python version
        std::vector<std::pair<size_t,size_t>> calc_clusters_for_threads();
        Node pop_possible_starting_nodes();
        void genetic_algorithm_driver();
        std::vector<std::vector<std::vector<Node>>> select_parents(std::vector<std::vector<Node>> &population);
        void calculate_all_fitness(std::vector<std::vector<Node>> &population);
};



#endif
