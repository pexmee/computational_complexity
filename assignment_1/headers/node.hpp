#ifndef NODE_HPP
#define NODE_HPP
#include <string.h>
#include <iostream>
#include <unordered_map>

struct Node{
        int x;
        int y;
        std::unordered_map<Node,double> weights;
        
        Node(); 
        ~Node();
        Node(std::string x, std::string y);
        
        bool operator==(const Node &other) const{
        return (x == other.x && y == other.y);
    }

};

template <>
struct std::hash<Node>{
    int x;
    int y;

    std::size_t operator()(const Node& node){
        using std::size_t;
        using std::hash;

        return ((std::hash<int>()(node.x) ^ (std::hash<int>()(node.y) << 1)) >> 1);
    }
};

#endif