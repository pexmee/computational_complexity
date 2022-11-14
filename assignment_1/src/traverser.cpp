#include "../headers/traverser.hpp"

double dist_calc(Node &current_node, Node &neighbor){
    return std::sqrt(std::pow((neighbor.x - current_node.x),2) + std::pow((neighbor.y - current_node.y),2));
}

size_t NUM_THREADS = 0;



/**
 * Rewrite this to use a genetic algorithm with the following approach:
 *
 *   Initialization: The initial solutions(first generations) are randomly initialized
 *   Fitness Score: Here, the main idea is, it may be hard to find the optimal solution but once we have some solution, it's easy to attach a goodness or fitness score to it.
 *   Selection: The fittest member (solution) of the population survives and moves on to the next generation.
 *   Crossover: The fittest member(solutions) of the population combines in pairs to create new members (a possible solution).
 *   Mutation: New members (Possible new solutions) change themselves in a small amount to get even better.
 * 
 */


 /**
  * 1. Initialization: Initialize by randomizing a few solutions - just pick a node at random until a cycle has been built. 
  * 2. Fitness score: The fitness score would be based upon the length traversed to build the cycle.
  * 3. Selection: Selection would be done by picking the top solutions, a multiple of 2 to make sure we can make children from them.
  * 4. Crossover: Combine the fittest solutions by  
  */


Traverser::Traverser(){
    this->nodes = {};
    this->cycle = {};
    this->possible_starting_nodes = {};
    this->current_node = nullptr;
    this->distance_traversed = 0;
    this->starting_nodes_set = false;
    this->fitness_scores = {};
}

Traverser::~Traverser(){   
    if(this->current_node != nullptr){
        delete this->current_node;
    }
}

void Traverser::traverse(){
    this->initialize_nodes();
    std::cout << "initialized nodes" << std::endl;

    if (this->starting_nodes_set && this->possible_starting_nodes.size() > 0){
        this->init_playground();
        this->randomize_parents_and_set_fitness();
    }
}


void Traverser::make_children(){

}

Node random_parent(){
}

std::vector<double> calc_probabilities(std::vector<double> &fitness_scores){
    double total_fitness = 0;
    std::vector<double> probabilities = {};

    for(unsigned i = 0; i < fitness_scores.size(); i++){
        total_fitness += fitness_scores[i];
    }
    for(unsigned i = 0; i < fitness_scores.size(); i++){
        probabilities.push_back(fitness_scores[i] / total_fitness);
    }
    return probabilities;
}

void Traverser::select_parents(){
    std::mt19937 gen(std::random_device{}());
    std::vector<double> probabilities = calc_probabilities(this->fitness_scores);
    std::discrete_distribution<std::size_t> d{probabilities.begin(), probabilities.end()};
    auto parents_a = this->parent_population[d(gen)];
    auto parents_b = this->parent_population[d(gen)];
    
}

void Traverser::randomize_parents_and_set_fitness(){
    int index, cnt = 0;
    std::ofstream r;
    r.open(CYCLE_FILE, std::ofstream::out | std::ofstream::trunc);
    r.close();
    while (cnt < 10){
        cnt++;
        while (this->nodes.size() > 1){
            index = rand()%(this->nodes.size());
            this->distance_traversed += dist_calc((*this->current_node),this->nodes[index]);
            delete this->current_node;
            this->current_node = new Node(this->nodes[index]);
            this->cycle.push_back(*this->current_node);
            this->nodes.erase(this->nodes.begin() + index);
        }
        this->fitness_scores.push_back(this->distance_traversed);
        this->close_cycle();
        this->initialize_nodes();
        this->init_playground();
    }
    this->set_fitness_scores();
    this->make_children();
    this->mutate();
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
            this->cycle.push_back(*this->current_node);
            
            // Erase the current node from the nodes
            for(unsigned i = 0; i < this->nodes.size(); i++){
                if (*(this->current_node) == this->nodes[i]){
                    std::cout << "removing node" << std::endl;
                    this->nodes.erase(this->nodes.begin() + i); // remove the node
                    break;
                }
            }
        }
    }
}

void Traverser::get_data(){
    std::ifstream data_file("../data128.txt");
    std::string line;

    while(getline(data_file,line)){
        int x, y;
        std::stringstream ss(line);
        ss >> x;
        ss >> y;
        Node node(x,y);
        if(std::find(this->nodes.begin(),this->nodes.end(), node) == this->nodes.end()){
            // File contained duplicates, so we remove them.
            this->nodes.push_back(node);
        }
    }
}

// For serializing the cycle to json object
__always_inline void to_json(nlohmann::json &j, const Node &node) {
    j = node.serialize();
}

__always_inline void from_json(nlohmann::json &j, Node &node){
    //
}



void Traverser::dump_cycle(){
    std::string start_coord = "(" + std::to_string(this->cycle[0].x) + "," + std::to_string(this->cycle[0].y) + ")";
    nlohmann::json data;
    data["cycle"] = this->cycle;
    data["dist"] = this->distance_traversed;
    data["start_coord"] = start_coord;
    
    // data[start_coord] = this->distance_traversed; // {this->distance_traversed, this->cycle};
    std::ofstream o, r;
    o.open(CYCLE_FILE, std::ios_base::app);
    o << data << std::endl;
    o.close();
}

void Traverser::mutate(){
    std::cout << "mutate called" << std::endl;
    nlohmann::json data;
    std::ifstream f(CYCLE_FILE);
    std::string line;
    while (std::getline(f, line)){
        std::stringstream ss(line);
        ss >> data;
        // std::cout << data["dist"] << std::endl;
    }

}

void Traverser::reset_traversal(){
    if(this->current_node != nullptr){
        delete this->current_node;
    }
    this->traverse();
}

void Traverser::close_cycle(){
    this->cycle.push_back(this->cycle[0]);
    this->distance_traversed += dist_calc(*this->current_node, this->cycle[0]);
    std::cout << "cycle len: " << this->cycle.size() << std::endl;
    std::cout << "distance traversed: " << this->distance_traversed << std::endl;
    this->parent_population.push_back(this->cycle);
    // this->dump_cycle();
    // this->reset_traversal();
    // exit(EXIT_SUCCESS);
}



std::vector<std::pair<size_t,size_t>> Traverser::calc_clusters_for_threads(){
    /*
    Calculates the size of the clusters for each thread.
    Returns a vector containing the start index and the stopping index
    for operations on this->nodes.
    */
    std::vector<std::pair<size_t,size_t>> vals = {};
    size_t size = this->nodes.size();

    if(size > 1000){
        NUM_THREADS = size / CLUSTER_SIZE;
        NUM_THREADS++;
        size_t length = size / NUM_THREADS;
        size_t remainder = size % NUM_THREADS;
        size_t chunk_a = length;
        size_t a_start = 0, a_stop = 0, b_start = 0, b_stop = 0;

        for (size_t i = 0; i < NUM_THREADS; i++){
            if(i < NUM_THREADS - remainder){
                a_start = ((i*chunk_a));
                a_stop = (chunk_a*(i+1))-1; // -1 due to indexing.
                vals.push_back(std::pair<size_t,size_t>(a_start,a_stop));

            }

            if(remainder && i >= NUM_THREADS-remainder){
                b_start = a_stop + (i%(NUM_THREADS-remainder))*(chunk_a+1)+1;
                b_stop = a_stop + ((i%(NUM_THREADS-remainder))+1)*(chunk_a+1);
                vals.push_back(std::pair<size_t,size_t>(b_start,b_stop));
            }
        }
        return vals;
    }
    else{
        vals.push_back(std::pair<size_t,size_t>(0,size-1));
        return vals;
    }
}
