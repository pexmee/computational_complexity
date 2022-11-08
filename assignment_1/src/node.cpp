#include "../headers/node.hpp"

Node::Node(){
    this->x = -1;
    this->y = -1;
    this->from = nullptr;
    this->to = nullptr;
    this->weight = 0;
    this->visited = false;
}

Node::~Node(){
    // We didn't dynamically allocate anything. Just default it.
}

Node::Node(int x, int y){
    this->x = x;
    this->y = y;
    this->from = nullptr;
    this->to = nullptr;
    this->weight = 0;
    this->visited = false;
}

