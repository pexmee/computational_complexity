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


/* Defines */
#define DATA128 "data128.txt"
#define DATA16K "data16k.txt"
#define CYCLE_FILE "cycles.ndjson"
#define MAX_X
#define MAX_Y
#define INT_MAX 2147483647
#define NUM_THREADS 20


class Traverser{

    /**
     * TODO: Multithread aquisition of the shortest distance.
     * This can be done by spawning "amount of nodes"/10, 
     * and then computing the shortest distance each cluster
     * and then finally joining the threads and comparing them
     * to find which was the ultimate choice.
     */
    
    private:
        std::vector<Node> nodes;
        std::vector<Node> cycle;
        std::vector<Node> possible_starting_nodes;
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
        Node closest(Node &closest_node, double &smallest_dist);
        void close_cycle();
        void reset_traversal();
        double calc_dist(Node* node);
        void closest_wrapper();
        void dump_cycle();
        void get_data(); // Need to be modified a bit from python version
        std::vector<std::pair<size_t,size_t>> calc_clusters_for_threads();
        void calc_closest_chunk(std::vector<std::pair<size_t,size_t>> &vals);
        Node pop_possible_starting_nodes();
};



#endif
