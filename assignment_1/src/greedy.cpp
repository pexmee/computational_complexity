#include "../headers/node.hpp"
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <stack>
#include <nlohmann/json.hpp>

#define INFILE "../data16k.txt"
#define OUTFILE "cycle16k.ndjson"

using namespace std;

__always_inline double dst(const Node &a, const Node &b){
    return sqrt(pow((a.x - b.x),2) + pow((a.y - b.y),2));
}

Node closest_node(Node &current, vector<Node> &nodes){
    pair<double,Node> closest = {INT32_MAX, Node()};
    stack<Node,vector<Node>> ns(nodes);

    double cdist;
    int i = 0;
    while (ns.size() > 0){

        if (ns.top() == current){
            ns.pop();
            i++;
            continue;
        }
        cdist = dst(current,ns.top());

        if (cdist < closest.first){
            closest.first = cdist;
            closest.second = ns.top();
            ns.pop();
            i++;
            continue;
        }
        else{
            ns.pop();
            i++;
            continue;
        }
    }
    nodes.erase(nodes.begin() + i);
    return closest.second;
}

double total_score(const vector<Node> &solution){
    double total = 0;

    for (size_t i = 0; i < solution.size()-1; i++){
        total += dst(solution[i],solution[i+1]);
    }
    total += dst(solution[0],solution[solution.size()-1]); // Because the last node will always have to connect back to the first.
    return total;
}

// For serializing the cycle to json object
__always_inline void to_json(nlohmann::json &j, const Node &node) {
    j = node.serialize();
}


int main(){
    ifstream data_file(INFILE);
    string line;
    vector<Node> nodes, solution;

    while (getline(data_file,line)){
        int x, y;
        stringstream ss(line);
        ss >> x;
        ss >> y;
        Node node(x,y);
        if (find(nodes.begin(), nodes.end(), node) == nodes.end()){
            nodes.push_back(node); // Only append unique nodes, ignore dupliates
        }
    }
    double best_score = INT32_MAX, current_score = 0;

    for (size_t i = 0; i < nodes.size(); i++){
        solution.clear();
        Node current = nodes[i]; // start node
        nodes.erase(nodes.begin() + i);
        solution.push_back(current);
        std::vector<Node> n = nodes;

        for (size_t j = 0; j < nodes.size(); j++){
            current = closest_node(current,n);
            solution.push_back(current);
        }
        current_score = total_score(solution);
        if (current_score < best_score){
            best_score = current_score;
            cout << "best score updated to: " << best_score << endl; 
        }
        
    }
    ofstream o;
    o.open(OUTFILE);
    time_t now;
    now = time(nullptr);
    char* stamp = ctime(&now);
    cout << stamp << "best solution is: " << fixed << best_score << endl;
    string start_coord = "(" + to_string(solution[0].x) + "," + to_string(solution[0].y) + ")";
    nlohmann::json data;
    data["dist"] = best_score;
    data["start_node"] = start_coord;
    data["timestamp"] = stamp;
    data["zcycle"] = solution;
    o << data << endl; 

    return EXIT_SUCCESS;
}
