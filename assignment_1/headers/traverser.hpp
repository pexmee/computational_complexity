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

class Traverser{
    
    private:
        std::vector<Node> nodes;
        std::vector<Node> cycle;
        std::vector<std::vector<Node>> parent_population;
        std::vector<Node> possible_starting_nodes;
        std::vector<double> fitness_scores;
        Node* current_node;
        double distance_traversed;
        bool starting_nodes_set;
        
    public:
        Traverser();
        ~Traverser();
        void graph();
        void traverse();
        void initialize_nodes();
        void init_playground();
        void weigh_nodes();
        void choose_shortest_path();
        void close_cycle();
        void reset_traversal();
        double calc_dist(Node &node);
        void closest_wrapper();
        void dump_cycle();
        void mutate();
        void set_fitness_scores();
        void randomize_parents_and_set_fitness();
        void select_parents();
        void make_children();
        void get_data(); // Need to be modified a bit from python version
        std::vector<std::pair<size_t,size_t>> calc_clusters_for_threads();
        Node pop_possible_starting_nodes();
        std::pair<int,double> closest_parallel(std::vector<std::pair<size_t,size_t>> &vals);
        std::pair<int,double> closest_linear(std::vector<std::pair<size_t,size_t>> &vals);
};



#endif
