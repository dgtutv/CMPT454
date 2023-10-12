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
    //If the node is a leaf node
    if(node->children.empty()){
        //If there is room, insert into the leaf node
        if(node->keyValues.size() < maxNumPointers - 1){
            node->keyValues.insert(pair<int, string>(key, value));
        } 
        //Otherwise
        else{
            //If possible, insert into parent and add a new pointer
            if(!node->parent->isFull()){
                node->parent->keyValues.insert(pair<int, string>(key, value));
                Node* newSibling = new Node(node->parent, true, this);
                allNodes.push_back(newSibling);

                //Find the corresponding pointer location
                int counter = 1;    //We want to insert to the right of the corresponding location
                for(map<int, string>::iterator it = node->parent->keyValues.begin(); it != node->parent->keyValues.end(); it++){
                    if(it->first == key){
                        node->parent->children[counter] = newSibling;
                        break;
                    }
                    counter++;
                }
                
                //Balance the node with it's newSibling 
                vector<int> keysToRemove;
                for(map<int, string>::iterator it = node->keyValues.begin(); it != node->keyValues.end(); it++){
                    if(it->first >= key){
                        newSibling->keyValues.insert(pair<int, string>(it->first, it->second));
                        keysToRemove.push_back(it->first);
                    }
                }

                //Remove keys that were inserted into the new sibling
                for(int i=0; i<keysToRemove.size(); i++){
                    node->keyValues.erase(keysToRemove[i]);
                }

                //Finally, add the key/value pair to the new sibling
                newSibling->keyValues.insert(pair<int, string>(key, value));
            }

            //If the parent is full,
            else{
                //If the parent is the root
                if(node->parent == root){
                    //Create a new root
                    Node* newRoot = new Node(NULL, false, this);
                    allNodes.push_back(newRoot);
                    pair<int, string> middlePair;

                    //Split the parent into two parent siblings (maybe uncles?)
                    Node* newUncle = new Node(newRoot, false, this);
                    allNodes.push_back(newUncle);
                    node->parent->parent = newRoot;

                    //Balance the parent's key/value pairs, and pointers with it's new sibling
                    int counter = 0;
                    vector<int> keysToRemove;
                    for(map<int, string>::iterator it=node->parent->keyValues.begin(); it!=node->parent->keyValues.end(); it++){
                        //All pointers and key/value pairs at and beyond the middle pair should be moved to the uncle
                        if(counter >= floor((maxNumPointers-1)/2)){     
                            middlePair = pair<int, string>(it->first, it->second);
                            keysToRemove.push_back(it->first);
                            newUncle->keyValues.insert(pair<int, string>(it->first, it->second));
                            newUncle->children.push_back(node->parent->children[counter]);
                            node->parent->children[counter] = NULL;
                        }
                        counter++;
                    }

                    //Remove the keys that were moved to the uncle from the original parent
                    for(int i=0; i<keysToRemove.size(); i++){
                        node->parent->keyValues.erase(keysToRemove[i]);
                    }

                    //Insert the middlePair to the new root
                    newRoot->keyValues.insert(middlePair);
                
                    //Distribute the new root's pointers
                    newRoot->children.push_back(node->parent);
                    newRoot->children.push_back(newUncle);
                    root = newRoot;

                    //Create a new sibling to our node
                    Node* newSibling = new Node(node->parent, true, this);
                    allNodes.push_back(newSibling);

                    //Find the corresponding pointer location in the parent for our newSibling
                    counter = 1;
                    for(map<int, string>::iterator it=node->parent->keyValues.begin(); it!=node->parent->keyValues.end(); it++){
                        if(it->first == key){
                            node->parent->children[counter] = newSibling;
                            break;
                        }
                        counter++;
                    }

                    //Balance the node with it's new sibling 
                    node->keyValues.insert(pair<int, string>(key, value));
                    keysToRemove.clear();
                    for(map<int, string>::iterator it = node->keyValues.begin(); it != node->keyValues.end(); it++){
                        if(it->first >= key){
                            newSibling->keyValues.insert(pair<int, string>(it->first, it->second));
                            keysToRemove.push_back(it->first);
                        }
                    }

                    //Remove keys that were inserted into the new sibling
                    for(int i=0; i<keysToRemove.size(); i++){
                        node->keyValues.erase(keysToRemove[i]);
                    }
                }
                    
                //Otherwise (if the parent is not the root),
                else{
                    //Update the parent pointer
                    Node* grandparent = node->parent->parent;

                    
                    //Split the parent into two siblings
                    //Create a new sibling to the original node
                    //Find the corresponding pointer location in the parent for our newSibling
                    //Balance the original node with it's new sibling 
                    //Remove keys that were inserted into the new sibling
                    //Add the key/value pair to the new sibling
                }
            }
        } 
    }

    //If the node is internal
    else{
        int counter = 0;
        for(map<int, string>::iterator it = node->keyValues.begin(); it != node->keyValues.end(); it++){
            //If there is a child which the key is less than it's values
            if(key < it->first){
                //Recursively call the function to insert the key/value
                insertInternal(node->children[counter], key, value);
                return;
            }
            counter++;
        }
        //If there is no child which the key is less than its values
            //If there is room in the node, add a new child, and its value
            //Otherwise, insert into the end pointer of the node
    }

    //Split the node into two children if necessary
    if(node->isOverflow()){
        map<int, string>::iterator middlePair = splitNode(node);
        Node* leftChild = node->children[0];
        Node* rightChild = node->children[1];

        //Decide which child to insert into 
        if(key < middlePair->first){
            leftChild->keyValues.insert(pair<int, string>(key, value));
        } 
        else{
            rightChild->keyValues.insert(pair<int, string>(key, value));
        }

        //Update the root if the node is the root
        if(node == root){
            Node* newRoot = new Node(nullptr, false, this);
            allNodes.push_back(newRoot);
            newRoot->children.push_back(leftChild);
            newRoot->children.push_back(rightChild);
            newRoot->keyValues.insert(*middlePair);
            updateRoot(newRoot);
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

