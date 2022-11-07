#include "../headers/node.hpp"

Node::Node(){
    // Defined for pointers that's all.
}

Node::~Node(){
    // We didn't dynamically allocate anything. Just default it.
}

Node::Node(std::string x, std::string y){
    this->x = std::stof(x);
    this->y = std::stof(y);
    this->weights = {};
}

