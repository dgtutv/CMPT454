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
/*--------------------------------------------------B+ Tree-----------------------------------------------------------*/
BPlusTree::BPlusTree(int maxNumPointers):maxNumPointers(maxNumPointers){};      //Constructor

bool BPlusTree::insert(int key, string value){
    if(root == NULL){     //If there is no root, make this the root
        rootNode newNode(key, value);
        root = &newNode;
        allNodes.push_back(&newNode);
    }
    else if(root->getChildren().size() == 0){   //If the root has no children
        //Make the root a child of itself
        leafNode rootAsLeaf(root->getKey(), root->getValue(), root);
        root->addChild((Node*)rootAsLeaf);
        //Make the new node a child of the root
    }
}
