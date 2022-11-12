#include "../headers/traverser.hpp"

void calc_wrapper(std::vector<Node> &nodes, std::vector<Node> &cycle, size_t start_idx, size_t stop_idx){

}

double dist_calc(Node &current_node, Node &neighbor){
    return std::sqrt(std::pow((neighbor.x - current_node.x),2) + std::pow((neighbor.y - current_node.y),2));
}

class thread_obj{

    public:
    void operator()(Node &current_node, std::vector<Node> &nodes, std::vector<Node> &cycle, size_t &start_idx, size_t &stop_idx, size_t &idx, std::pair<int, double> (&indices_and_distances)[NUM_THREADS])
    {
        double smallest_dst = INT_MAX;
        int smallest_index = -1;

        for(int i = 0; i < nodes.size(); i++){
            if((std::find(cycle.begin(), cycle.end(), nodes[i])) == cycle.end()){
                double cmp_dist = dist_calc(current_node, nodes[i]);
                if (cmp_dist < smallest_dst){
                    smallest_dst = cmp_dist;
                    smallest_index = i;
                }
            }
            
        }
        indices_and_distances[idx] = std::pair<double,int>(smallest_dst,smallest_index);
    }
};

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
    // Todo call function to spawn threads and compute this stuff.
    double shortest_dist;
    Node asdf(-1,-1);
    Node closest_node = this->closest(asdf, shortest_dist);

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


Node Traverser::closest(Node &closest_node, double &smallest_dist){
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


void Traverser::closest_wrapper(){

}



void Traverser::calc_closest_chunk(std::vector<std::pair<size_t,size_t>> &vals){
    std::vector<std::thread> threads = {};
    // index of what it found as smallest, and the distance
    std::pair<int,double> indices_and_distances[NUM_THREADS]; 

    // Spawn threads to calculate closest node for each cluster on this->nodes
    for(size_t i = 0; i < vals.size(); i++){
        std::thread th(thread_obj(), std::ref(this->current_node), std::ref(this->nodes), std::ref(this->cycle), std::ref(vals[i].first), std::ref(vals[i].second));
        threads.push_back(std::move(th));
    }
    for(size_t i = 0; i < vals.size(); i++){
        threads[i].join();
    }
}



std::vector<std::pair<size_t,size_t>> Traverser::calc_clusters_for_threads(){
    /*
    Calculates the size of the clusters for each thread.
    Returns a vector containing the start index and the stopping index
    for operations on this->nodes.
    */
    
    std::vector<std::pair<size_t,size_t>> vals = {};
    size_t size = this->nodes.size();
    size_t length = size / NUM_THREADS;
    size_t remainder = size % NUM_THREADS;
    size_t chunk_a = length, chunk_b = length +1;
    size_t a_start = 0, a_stop = 0, b_start = 0, b_stop = 0;

    for (size_t i = 0; i < NUM_THREADS; i++){
        if(i < NUM_THREADS - remainder){
            a_start = ((i*chunk_a));
            a_stop = (chunk_a*(i+1))-1; // -1 due to indexing.
            vals.push_back(std::pair<size_t,size_t>(a_start,a_stop));

        }
        size_t tmp_a = a_start, tmp_b = b_start;
        if(remainder && i >= NUM_THREADS-remainder){
            b_start = a_stop + (i%(NUM_THREADS-remainder))*(chunk_a+1)+1;
            b_stop = a_stop + ((i%(NUM_THREADS-remainder))+1)*(chunk_a+1);
            vals.push_back(std::pair<size_t,size_t>(b_start,b_stop));
        }
    }
    return vals;
}
