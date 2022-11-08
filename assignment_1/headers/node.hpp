#ifndef NODE_HPP
#define NODE_HPP
#include <string.h>
#include <iostream>
#include <algorithm>

struct Node{
        int x;
        int y;
        bool visited;
        double weight;
        Node *from;
        Node *to;
        
        Node(); 
        ~Node();
        Node(int x, int y);
        
        Node(const Node &other){
            this->x = other.x;
            this->y = other.y;
            this->weight = other.weight;
            this->from = other.from;
            this->to = other.to;
        }
        Node& operator=(const Node &other){
            this->x = other.x;
            this->y = other.y;
            this->weight = other.weight;
            this->from = other.from;
            this->to = other.to;
            return *this;
        }

        bool operator==(const Node &other) const{
        return (this->x == other.x && this->y == other.y);
        }

        bool operator!=(const Node &other) const {
            return (this->x != other.x || this->y != other.y);
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