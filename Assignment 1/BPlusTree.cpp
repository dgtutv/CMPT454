#include "BPlusTree.h"
#include <algorithm>
using namespace std;

bool compareNodes(const Node* a, const Node* b){     //This function is used to compare Node's keys
    return a->getValue() > b->getValue();
}

/*---------------------------------------------------Node--------------------------------------------------------*/
Node::Node(int key, string value, int maxNumPointers) : key(key), value(value), maxNumPointers(maxNumPointers){     //Constructor
    this->isLeaf = false;
    this->isRoot = false;
    this->isInterior = false;
};   

string Node::getValue() const{      
    return this->value;
}

int Node::getKey() const{
    return this->key;
}

list<Node*> Node::getChildren() const{
    return this->children;
}

Node* Node::getParent() const{
    return this->parent;
}

Node* Node::setParent(Node* newParent){
    Node* returnParent = this->parent;
    this->parent = newParent;
    return(returnParent);
}

bool Node::addChild(Node* newChild){
    if(this->children.size() < this->maxNumPointers){   //First check if there is room for the child
        //Then check if a node with the same value is already present
        for(auto iterator = this->children.begin(); iterator !=this->children.end(); ++iterator){
            Node* currentChild = *iterator;
            if(currentChild->getKey() == newChild->getKey()){
                return false;
            }
        }
        //If not, add it and sort the children list
        this->children.push_back(newChild);
        this->children.sort(compareNodes);
        return true;
    }
    return false;
}

Node* Node::removeChild(int value){
    for(auto iterator = this->children.begin(); iterator != this->children.end(); ++iterator){
        Node* currentChild = *iterator;
        if(currentChild->getKey() == value){    //If the child is found
            this->children.erase(iterator); //Remove the child
            return currentChild;
        }
    }
    //Otherwise, report failure
    return NULL;
}
/*--------------------------------------------------B+ Tree-----------------------------------------------------------*/
BPlusTree::BPlusTree(int maxNumPointers):maxNumPointers(maxNumPointers){};      //Constructor

//TODO: root can contain up to maxNumPointers, use a list in B+ tree

bool BPlusTree::insert(int key, string value){
    Node newNode(key, value, maxNumPointers);
    allNodes.push_back(&newNode);
    if(root == NULL){     //If there is no root, make this the root
        newNode.isRoot = true;
        newNode.isLeaf = true;
        root = &newNode;
    }
    else if(root->getChildren().size() == 0){   //If the root has no children
        root->addChild(root);   //Make the root a child of itself
        root->addChild(&newNode);    //Make the new node a child of the root
    }
}
