#include "../headers/traverser.hpp"

double dist_calc(Node &current_node, Node &neighbor){
    return std::sqrt(std::pow((neighbor.x - current_node.x),2) + std::pow((neighbor.y - current_node.y),2));
}

size_t NUM_THREADS = 0;



class index_distance_th{

    public:
    void operator()(Node &current_node, std::vector<Node> &nodes, size_t &start_idx, 
    size_t &stop_idx, std::pair<int, double> &index_and_distance)
    {
        /**
         * Iterates over the nodes, getting the smallest distanc eof current chunk. Saves result in 
         * indices_and_distances by appending the distance and the index.
         */
        double smallest_dst = INT_MAX;
        int smallest_index = -1;

        for(size_t i = start_idx; i < stop_idx; i++){
            if(nodes[i] != current_node){
                
                double cmp_dist = dist_calc(current_node, nodes[i]);
                if (cmp_dist < smallest_dst){
                    smallest_dst = cmp_dist;
                    smallest_index = i;
                }
            }
            
        }
        index_and_distance = std::pair<int,double>(smallest_index,smallest_dst);
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
    delete this->current_node;
}

void Traverser::traverse(){
    this->initialize_nodes();
    std::cout << "initialized nodes" << std::endl;

    if (this->starting_nodes_set && this->possible_starting_nodes.size() > 0){
        this->init_playground();
        while (this->nodes.size() > 0){
            this->choose_shortest_path();
        }
    }
}

std::pair<int,double> Traverser::closest_linear(std::vector<std::pair<size_t,size_t>> &vals){
    double smallest_dst = INT_MAX;
    int smallest_index = -1;

    for(size_t i = vals[0].first; i < vals[0].second; i++){
        if(this->nodes[i] != *(this->current_node)){
            
            double cmp_dist = dist_calc(*this->current_node, nodes[i]);
            if (cmp_dist < smallest_dst){
                smallest_dst = cmp_dist;
                smallest_index = i;
            }
        }
        
    }
    return std::pair<int,double>(smallest_index,smallest_dst);
}

void Traverser::choose_shortest_path(){
    // Todo call function to spawn threads and compute this stuff.
    std::pair<int,double> index_distance = {};
    std::vector<std::pair<size_t,size_t>> vals = calc_clusters_for_threads();
    if (this->nodes.size() > 1000){
        index_distance = this->closest_parallel(vals);
    }
    else{
        index_distance = this->closest_linear(vals);
    }

    if (index_distance.first == -1){
        std::cout << "no closest dist, closing cycle" << std::endl;
        this->close_cycle(); // Since we couldn't find a shortest path we assume we are done here.
        return;
    }
    
    delete this->current_node;
    this->current_node = new Node(this->nodes[index_distance.first]);
    this->nodes.erase(this->nodes.begin() + index_distance.first);
    this->cycle.push_back(*(this->current_node));    
    this->distance_traversed += index_distance.second;
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
void to_json(nlohmann::json &j, const Node &node) {
    j = node.serialize();
}

void Traverser::dump_cycle(){
    std::string start_coord = "(" + std::to_string(this->cycle[0].x) + "," + std::to_string(this->cycle[0].y) + ")";
    nlohmann::json data;
    data[start_coord] = this->distance_traversed; // {this->distance_traversed, this->cycle};
    std::ofstream o;
    o.open("cycles_16k.ndjson", std::ios_base::app);
    o << data << std::endl;
}

void Traverser::reset_traversal(){
    this->traverse();
}

void Traverser::close_cycle(){
    this->cycle.push_back(this->cycle[0]);
    this->distance_traversed += dist_calc(*this->current_node, this->cycle[0]);
    std::cout << "cycle len: " << this->cycle.size() << std::endl;
    std::cout << "distance traversed: " << this->distance_traversed << std::endl;
    this->dump_cycle();
    this->reset_traversal();
    // exit(EXIT_SUCCESS);
}



std::pair<int,double> Traverser::closest_parallel(std::vector<std::pair<size_t,size_t>> &vals){
    std::vector<std::thread> threads = {};
    
    // index of what it found as smallest, and the distance
    std::pair<int,double> indices_and_distances[NUM_THREADS]; 
    
    // Spawn threads to calculate closest node for each cluster on this->nodes
    for(size_t i = 0; i < vals.size(); i++){
        std::thread th(index_distance_th(), std::ref(*this->current_node), std::ref(this->nodes), std::ref(vals[i].first), std::ref(vals[i].second), std::ref(indices_and_distances[i]));
        threads.push_back(std::move(th));
    }
    
    // We gots to join them before we can do our operations
    std::pair<int,double> smallest_dist = {-1,INT_MAX};
    for(size_t i = 0; i < vals.size(); i++){
        threads[i].join();
        if (indices_and_distances[i].second < smallest_dist.second){
            smallest_dist = indices_and_distances[i];
        }
    }

    return smallest_dist;
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
