#include "../headers/traverser.hpp"

Traverser::Traverser(){
    this->stack = {};
    this->nodes = {};
    this->possible_starting_nodes = {};
    this->cycle = {};
    this->current_node = nullptr;
    this->distance_traversed = 0;
    this->idx = -1;
    this->starting_nodes_set = false;
}

Traverser::~Traverser(){
    delete this->current_node;
}

void Traverser::traverse(){

}

void Traverser::make_stack(){

}

void Traverser::weigh_nodes(){

}

void Traverser::choose_shortest_path(){

}

Node Traverser::smallest(Node* node){
    double min_weight = -1;
    bool once = true;
    std::unordered_map<Node,double>::iterator idx;

    if (node->weights.size() != 0){
        for (std::unordered_map<Node,double>::iterator it = node->weights.begin(); it != node->weights.end(); it++){
            if (once){
                min_weight = it->second;
            }
            if(it->second < min_weight){
                min_weight = it->second;
                idx = it;
            }
        }
        if (idx != node->weights.end()){
            return idx->first;
        }
    }
    return Node("-1","-1");
}

void Traverser::initialize_nodes(){

}

void Traverser::graph(){

}

double Traverser::calc_dist(Node* node){
    return std::sqrt(std::pow((node->x - this->current_node->x),2) + std::pow((node->y - this->current_node->y),2));
}

std::vector<std::string> Traverser::get_data(){

}

void Traverser::dump_cycle(){

}

void Traverser::reset_traversal(){

}

void Traverser::close_cycle(){

}