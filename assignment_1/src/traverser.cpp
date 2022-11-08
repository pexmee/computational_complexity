#include "../headers/traverser.hpp"

Traverser::Traverser(){
    this->nodes = {};
    this->cycle = {};
    this->possible_starting_nodes = {};
    this->current_node = nullptr;
    this->distance_traversed = 0;
    this->starting_nodes_set = false;
}

Traverser::~Traverser(){
    std::cout << "destructor called for current node" << std::endl;
    delete this->current_node;
}

void Traverser::traverse(){
    this->initialize_nodes();
    std::cout << "initialized nodes" << std::endl;

    if (this->starting_nodes_set && this->possible_starting_nodes.size() > 0){
        while (this->nodes.size() > 0){
            this->init_playground();
            this->choose_shortest_path();
        }
        std::cout << "closing cycle.." << std::endl;
        this->close_cycle();
    }
}


void Traverser::weigh_nodes(){

}

void Traverser::choose_shortest_path(){
    double shortest_dist;
    Node closest_node = this->closest(shortest_dist);

    // If it exists. This should probably be done with pointers tho
    // But that we will revise in the future. Kek.
    if (closest_node.x != -1 && closest_node.y != -1){
        this->distance_traversed += shortest_dist;
        
        for(unsigned i = 0; i < this->nodes.size(); i++){
            if(closest_node == this->nodes[i]){
                this->current_node = new Node(this->nodes[i]);
                this->nodes.erase(this->nodes.begin() + i);
                this->cycle.push_back(*(this->current_node));
                break;
            }
        }
    }
    else{
        std::cout << "no shortest path, cycle should be done" << std::endl;
    }
}

void Traverser::initialize_nodes(){
    // We do this in case we are traversing from another starting node.
    this->nodes.clear();
    this->cycle.clear();
    this->distance_traversed = 0;
    this->get_data();
    

    if (!this->starting_nodes_set){
        // std::cout << "this nodes size: " << this->nodes.size() << std::endl;
        this->possible_starting_nodes = this->nodes;
        this->starting_nodes_set = true;
        // std::cout << "starting nodes set: " << this->possible_starting_nodes.size() << std::endl;
    }
}

Node Traverser::pop_possible_starting_nodes(){
    Node ret = this->possible_starting_nodes[this->possible_starting_nodes.size()-1];
    this->possible_starting_nodes.pop_back();
    return ret;
}

void Traverser::init_playground(){
    if (this->nodes.size() > 0){ // Sanity check
        if (this->possible_starting_nodes.size() > 0 && this->cycle.size() == 0){
            this->current_node = new Node(this->pop_possible_starting_nodes());
            
            // Erase the current node from the nodes
            for(unsigned i = 0; i < this->nodes.size(); i++){
                if (*(this->current_node) == this->nodes[i]){
                    this->nodes.erase(this->nodes.begin() + i); // remove the node
                    break;
                }
            }
        }
    }
}


Node Traverser::closest(double &smallest_dist){
    smallest_dist = INT_MAX;
    Node closest_node(-1,-1);
    for (unsigned i = 0; i < this->nodes.size(); i++){
        
        // If the node is not found in this->cycle and the current node isn't the node we are looking at
        if((std::find(this->cycle.begin(), this->cycle.end(), this->nodes[i]) 
            == this->cycle.end()) && this->nodes[i] != *(this->current_node)){
                double cmp_distance = this->calc_dist(&(this->nodes[i]));

                if (cmp_distance < smallest_dist){
                    smallest_dist = cmp_distance;
                    closest_node = this->nodes[i];
                }
        }
    }
    return closest_node;
}

void Traverser::graph(){

}

double Traverser::calc_dist(Node* node){
    return std::sqrt(std::pow((node->x - this->current_node->x),2) + std::pow((node->y - this->current_node->y),2));
}

void Traverser::get_data(){
    std::ifstream data_file("../data16k.txt");
    std::string line;

    while(getline(data_file,line)){
        int x, y;
        std::stringstream ss(line);
        ss >> x;
        ss >> y;
        this->nodes.push_back(Node(x,y));
    }
}

void Traverser::dump_cycle(){

}

void Traverser::reset_traversal(){

}

void Traverser::close_cycle(){
    this->cycle.push_back(this->cycle[0]);
    this->distance_traversed += this->calc_dist(&(this->cycle[0]));
    std::cout << "cycle len: " << this->cycle.size() << std::endl;
    std::cout << "distance traversed: " << this->distance_traversed << std::endl;
    // this->dump_cycle();
}