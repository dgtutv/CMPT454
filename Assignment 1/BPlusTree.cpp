#include "BPlusTree.h"
#include <algorithm>
using namespace std;

bool compareNodes(const Node* a, const Node* b){     //This function is used to compare Node's keys
    return a->getValue() > b->getValue();
}

/*---------------------------------------------------Node--------------------------------------------------------*/
Node::Node(int key, string value) : key(key), value(value){};   //Constructor

string Node::getValue() const{      
    return this->value;
}

int Node::getKey() const{
    return this->key;
}

/*-------------------------------------------------Leaf Node-----------------------------------------------------*/
leafNode::leafNode(int key, string value, Node* parent) : Node(key, value), parent(parent){};       //Constructor

Node* leafNode::getParent() const{
    return this->parent;
}

list<leafNode*> leafNode::getSiblings() const{
    return this->siblings;
}

void leafNode::addSibling(leafNode* sibling){
    if(find(this->siblings.begin(), this->siblings.end(), sibling) == this->siblings.end()){   //Check if the node is already one of the siblings
        this->siblings.push_back(sibling);  //If not, add it to the list
        this->siblings.sort(compareNodes);  //Sort the list by node's keys
    }
}

Node* leafNode::changeParent(Node* newParent){
    Node* oldParent = this->parent;
    this->parent = newParent;
    return oldParent;
}

Node* leafNode::removeSibling(leafNode* sibling){
    if(find(this->siblings.begin(), this->siblings.end(), sibling) != this->siblings.end()){    //If the node is in the siblings list
        this->siblings.remove(sibling);      //Remove it
        return sibling;     //Return a pointer to it
    }
    return NULL;
}

/*-----------------------------------------------Interior Node---------------------------------------------------*/
interiorNode::interiorNode(int key, string value, Node* parent) : Node(key, value), parent(parent){};       //Constructor

list<interiorNode*> interiorNode::getSiblings() const{
    return this->siblings;
}

void interiorNode::addSibling(interiorNode* sibling){
    if(find(this->siblings.begin(), this->siblings.end(), sibling) == this->siblings.end()){   //Check if the node is already one of the siblings
        this->siblings.push_back(sibling);  //If not, add it to the list
        this->siblings.sort(compareNodes);  //Sort the list by node's keys
    }
}

Node* interiorNode::changeParent(Node* newParent){
    Node* oldParent = this->parent;
    this->parent = newParent;
    return oldParent;
}

Node* interiorNode::removeSibling(interiorNode* sibling){
    if(find(this->siblings.begin(), this->siblings.end(), sibling) != this->siblings.end()){    //If the node is in the siblings list
        this->siblings.remove(sibling);      //Remove it
        return sibling;     //Return a pointer to it
    }
    return NULL;
}
Node* interiorNode::removeChild(Node* child){
    if(find(this->children.begin(), this->children.end(), child) != this->children.end()){      //If the node is in the children list
        this->children.remove(child);
        return child;
    }
    return NULL;
}