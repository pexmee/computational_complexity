#include "../headers/traverser.hpp"

double dist_calc(Node &current_node, Node &neighbor){
    return std::sqrt(std::pow((neighbor.x - current_node.x),2) + std::pow((neighbor.y - current_node.y),2));
}

size_t NUM_THREADS = 0;
double CURRENT_BEST = INT_MAX;


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
    this->parent_population = {};
    this->new_generation = {};
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

std::vector<std::vector<std::vector<Node>>> Traverser::select_parents(std::vector<std::vector<Node>> &population){
    std::mt19937 gen(std::random_device{}());
    std::vector<double> probabilities = calc_probabilities(this->fitness_scores);
    std::discrete_distribution<std::size_t> d{probabilities.begin(), probabilities.end()};
    std::vector<std::vector<Node>> parents_a, parents_b;
    for(unsigned i = 0; i < probabilities.size(); i++){
        parents_a.push_back(population[d(gen)]);
        parents_b.push_back(population[d(gen)]);
    }
    std::vector<std::vector<std::vector<Node>>> ret = {parents_a, parents_b};

    return ret;
}

std::vector<Node> make_child(std::vector<Node> &parent_a, std::vector<Node> &parent_b){
    std::vector<Node> child = {};
    for(unsigned i = 0; i< parent_a.size()/3; i++){
        child.push_back(parent_a[i]);
    }

    for (unsigned i = 0; i < parent_b.size(); i++){
        // If the node isn't found in the child
        if(std::find(child.begin(), child.end(),parent_b[i]) == child.end()){
            child.push_back(parent_b[i]);
        }
    }
    return child;
}



void Traverser::mutate(){
    for (unsigned i = 0; i < this->new_generation.size(); i++){
        for (unsigned k = 0; k < int(MUTATION_RATE * this->nodes.size()); k++){
            int a = rand()%(this->nodes.size()-1);
            int b = rand()%(this->nodes.size()-1);
            Node tmpa = this->new_generation[i][a], tmpb = this->new_generation[i][b];
            this->new_generation[i][a] = tmpb;
            this->new_generation[i][b] = tmpa; 
        }
    }
}

void Traverser::copulate(std::vector<std::vector<std::vector<Node>>> &parents){
    std::vector<std::vector<Node>> temp_new_generation;

    for (unsigned i = 0; i < parents[0].size(); i++){ // Since they are both the same size anyway.
        std::vector<Node> child = make_child(parents[0][i], parents[1][i]);
        temp_new_generation.push_back(child);
    }
    this->new_generation = temp_new_generation;
}


std::pair<int,double> find_best_index_and_fitness(std::vector<double> &fitness_scores){
    std::pair<int,double> best_fitness = std::pair<int,double>(-1,INT_MAX);
    for (unsigned i = 0; i < fitness_scores.size(); i++){
        if(fitness_scores[i] < best_fitness.second){
            best_fitness.second = fitness_scores[i];
            best_fitness.first = i;
        }
    }
    return best_fitness;
}

void Traverser::genetic_algorithm_driver(){
    int n = 0;
    this->initialize_nodes();
    this->randomize_parents();
    this->calculate_all_fitness(this->parent_population);
    std::vector<std::vector<std::vector<Node>>> parents = this->select_parents(this->parent_population);
    this->copulate(parents);
    this->mutate();
    std::pair<int,double> best = find_best_index_and_fitness(this->fitness_scores);
    this->dump_cycle(best.second, this->new_generation[best.first]);
    
    while(true){
        n++;
        this->calculate_all_fitness(this->new_generation);   
        parents = this->select_parents(this->new_generation);
        this->copulate(parents);
        this->mutate();
        std::pair<int,double> best = find_best_index_and_fitness(this->fitness_scores);
        if(best.second < CURRENT_BEST){
            CURRENT_BEST = best.second;
            std::cout << "Found new shorter cycle " << CURRENT_BEST << "\n";
            this->dump_cycle(best.second, this->new_generation[best.first]);
        }
    }
}


void Traverser::calculate_all_fitness(std::vector<std::vector<Node>> &population){
    this->fitness_scores.clear();

    for (unsigned i = 0; i < population.size(); i++){
        this->distance_traversed = 0;

        for(unsigned k = 0; k < population[i].size()-1; k++){ 
            this->distance_traversed += dist_calc(population[i][k],population[i][k+1]);
        }
        // To add the fitness for the last node which is supposed to connect to the first node
        this->distance_traversed += dist_calc(population[i][0],population[i][population[i].size()-1]);
        this->fitness_scores.push_back(this->distance_traversed);
    }
    
}

void Traverser::randomize_parents(){
    int index, cnt = 0;
    // std::ofstream r;
    // r.open(CYCLE_FILE, std::ofstream::out | std::ofstream::trunc); // To wipe the file for each new iteration.
    // r.close();
    while (cnt < POPULATION_SIZE){
        cnt++;
        while (this->nodes.size() > 1){
            index = rand()%(this->nodes.size());
            this->current_node = new Node(this->nodes[index]);
            this->cycle.push_back(*this->current_node);
            // delete this->current_node;
            this->nodes.erase(this->nodes.begin() + index);
        }
        this->parent_population.push_back(this->cycle);
        this->initialize_nodes();
    }
    std::cout << parent_population.size() << std::endl;
}

void Traverser::initialize_nodes(){
    // We do this in case we are traversing from another starting node.
    this->nodes.clear();
    this->cycle.clear();
    this->distance_traversed = 0;
    this->get_data();
}

void Traverser::get_data(){
    std::ifstream data_file("../data16k.txt");
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

// __always_inline void from_json(nlohmann::json &j, Node &node){
    //
// }



void Traverser::dump_cycle(double fitness, std::vector<Node> &cycle){
    std::string start_coord = "(" + std::to_string(cycle[0].x) + "," + std::to_string(cycle[0].y) + ")";
    nlohmann::json data;
    data["dist"] = fitness;
    data["start_coord"] = start_coord;
    data["zcycle"] = cycle; // zcycle to order it to the back of the ndjson
    
    // data[start_coord] = this->distance_traversed; // {this->distance_traversed, this->cycle};
    std::ofstream o, r;
    o.open(CYCLE_FILE, std::ios_base::app);
    o << data << std::endl;
    o.close();
}



void Traverser::close_cycle(){
    this->cycle.push_back(this->cycle[0]);
    this->distance_traversed += dist_calc(*this->current_node, this->cycle[0]);
    std::cout << "cycle len: " << this->cycle.size() << std::endl;
    std::cout << "distance traversed: " << this->distance_traversed << std::endl;
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
