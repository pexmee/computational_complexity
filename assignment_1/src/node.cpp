#include "../headers/node.hpp"

Node::Node(){
    this->x = -1;
    this->y = -1;
}

Node::~Node(){
    // We didn't dynamically allocate anything. Just default it.
}

Node::Node(int x, int y){
    this->x = x;
    this->y = y;
}

