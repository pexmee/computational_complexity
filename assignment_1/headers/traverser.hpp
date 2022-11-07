#ifndef TRAVERSER_HPP
#define TRAVERSER_HPP

/* Includes */
#include "node.hpp"
#include <vector>
#include <cmath>
#include <iterator>

/* Defines */
#define DATA128 = "data128.txt"
#define DATA16K = "data16k.txt"
#define CYCLE_FILE = "cycles.ndjson"
#define MAX_X
#define MAX_Y



class Traverser{
    private:
        std::vector<Node> stack;
        std::vector<Node> nodes;
        std::vector<Node> cycle;
        std::vector<Node> possible_starting_nodes;
        std::vector<Node> starting_nodes;
        Node* current_node;
        double distance_traversed;
        int idx;
        bool starting_nodes_set;
        
    public:
        Traverser();
        ~Traverser();
        void graph();
        void traverse();
        void initialize_nodes();
        void make_stack();
        void weigh_nodes();
        void choose_shortest_path();
        Node smallest(Node* node);
        void close_cycle();
        void reset_traversal();
        double calc_dist(Node* node);
        void dump_cycle();
        std::vector<std::string> get_data(); // Need to be modified a bit from python version
};

#endif
