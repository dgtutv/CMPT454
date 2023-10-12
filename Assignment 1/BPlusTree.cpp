#include "BPlusTree.h"
#include <algorithm>
#include <math.h>
#include <iostream>
using namespace std;

/*------------------------------------------Node----------------------------------------*/
Node::Node(Node* parent, bool isLeaf, BPlusTree* tree) : parent(parent), isLeaf(isLeaf), tree(tree){};

bool Node::isOverflow() const{
    return keyValues.size() > tree->getMaxNumPointers() -1;
}

bool Node::isFull() const{
    return keyValues.size() >= tree->getMaxNumPointers()-1;
}

/*----------------------------------------BPlusTree-------------------------------------*/
BPlusTree::BPlusTree(int maxNumPointers) : maxNumPointers(maxNumPointers){};

int BPlusTree::getMaxNumPointers(){
    return maxNumPointers;
}

void BPlusTree::updateRoot(Node* newRoot){
    root = newRoot;
}

void BPlusTree::handleNodeOverflow(Node* node, int key, string value){
    if(node != NULL){
        //If there is room in the node, insert the key/value pair 
        if(!node->isFull()){
            node->keyValues.insert(pair<int, string>(key, value));
        }

        //Otherwise,
        else{
            //Split the node
            map<int, string>::iterator middlePair = splitNode(node);
            Node* leftChild = node->children[0];
            Node* rightChild = node->children[1];

            //Recursively inserts the key into the children
            if(key < middlePair->first){
                handleNodeOverflow(leftChild, key, value);
            }
            else{
                handleNodeOverflow(rightChild, key, value);
            }

            //Once split, check if the parent is also full
            if(node->parent->isFull()){
                //If so, recursively propogate the middle pair to the parent
                handleNodeOverflow(node->parent, middlePair->first, middlePair->second);
            }   
        }
    }
}

//Each interior/root node must contain floor(maxNumPointers/2) pointers. => floor(maxNumPointers-1/2) values
bool BPlusTree::insert(int key, string value){
    //TODO: check for duplicates first!!
    //TODO: make sure leaf nodes point to each other!

    if(root == NULL){   //If the root does not exist, enter the value into it
        root = new Node(NULL, true, this);
        allNodes.push_back(root);
        root->keyValues.insert(pair<int, string>(key, value));
    }
    else if(root->children.size() == 0){    //Check the root has no children
        if(!root->isFull()){      //If the root is not full, insert into the root
            root->keyValues.insert(pair<int, string>(key, value));
        }
        else{   //If the root is full
            insertInternal(root, key, value);
        }
    }
    //Make some sort of recursive function that can call on the root's children
    return false;
}  

void BPlusTree::insertInternal(Node* node, int key, string value){
    //If the node is a leaf, simply call our recursiveInsertion method
    if(node->children.empty()){
        handleNodeOverflow(node, key, value);
    }
    //Otherwise,
    else{
        //Check if there is a child which the key should go in
        int counter =  0;
        for(map<int, string>::iterator it = node->keyValues.begin(); it != node->keyValues.end(); it++){
            //If so, recursively call insertInteral to insert the value into the child
            if(key < it->first){
                insertInternal(node->children[counter], key, value);
                return;
            }
            counter++;
        }

        //If not, check if there is room in the node
        if(!node->isOverflow()){
            handleNodeOverflow(node, key, value);       //If there is no room in the node, handle the overflow
            return;
        }
        else{
            insertInternal(node->children.back(), key, value);      //If there is room, insert into the end pointer of the node
            return;
        }
    }
}

 map<int, string>::iterator BPlusTree::splitNode(Node* parent){     //Splits a parent node into two children and distributes the values, do not use for leaves
    parent->isLeaf = false;
    Node* leftChild = new Node(parent, true, this);
    allNodes.push_back(leftChild);
    Node* rightChild = new Node(parent, true, this);
    allNodes.push_back(rightChild);
    parent->children.push_back(leftChild);
    parent->children.push_back(rightChild);
    map<int, string>::iterator middlePair;
    int counter = 0;

    //Distribute the key/values
    for(map<int, string>::iterator it = parent->keyValues.begin(); it != parent->keyValues.end(); it++){  
        if(counter < floor((maxNumPointers-1)/2)){
            leftChild->keyValues[it->first] = it->second;
        }
        else{
            rightChild->keyValues[it->first] = it->second;
        }
        if(counter == floor((maxNumPointers-1)/2)){middlePair = it;}
        counter++;
    }

    //Remove all the key/values that are not the middle one
    for (map<int, string>::iterator it = parent->keyValues.begin(); it != parent->keyValues.end();) {
        if(it != middlePair){
            parent->keyValues.erase(it++);
        } 
        else{
            ++it;
        }
    }

    return middlePair;
}

void BPlusTree::printKeys(){
    printNodeKey(root);
    cout<<endl;
    for(vector<Node*>::iterator it = root->children.begin(); it != root->children.end(); it++){
        printNodeKey(*it);
    }
    cout<<endl;
    cout<<endl;
}

void BPlusTree::printNodeKey(Node* node){
    cout<<"|";
    for(map<int, string>::iterator it = node->keyValues.begin(); it != node->keyValues.end(); it++){
        cout<<it->first;
        if(it != prev(node->keyValues.end())){
            cout<<" ";
        }
    }
    cout<<"|";
}

//Tester code
int main(int argc, char const *argv[]){
    BPlusTree* tree = new BPlusTree(4);
    tree->insert(21, "21");
    tree->insert(2, "2");
    tree->insert(11, "11");
    tree->printKeys();
    tree->insert(8, "8");
    tree->printKeys();
    tree->insert(64, "64");
    tree->insert(5, "5");
    tree->printKeys();
}

