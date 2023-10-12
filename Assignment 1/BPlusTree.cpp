#include "BPlusTree.h"
#include <algorithm>
#include <math.h>
#include <iostream>
#include <queue>
using namespace std;

/*------------------------------Compare function for nodes-------------------------------*/
bool compareNodes(const BPlusTree::Node* a, const BPlusTree::Node* b){
    if (!a->keyValues.empty() && !b->keyValues.empty()) {
        return a->keyValues.begin()->first < b->keyValues.begin()->first;
    }
    return false;       //Return false if one or both of the nodes has no keys
}

/*------------------------------------------Node----------------------------------------*/
BPlusTree::Node::Node(Node* parent, bool isLeaf, BPlusTree* tree) : parent(parent), isLeaf(isLeaf), tree(tree){};

bool BPlusTree::Node::isOverflow() const{
    return keyValues.size() > tree->getMaxNumPointers() -1;
}

bool BPlusTree::Node::isFull() const{
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

bool BPlusTree::handleNodeOverflow(Node* node, int key, string value){
    if(node != nullptr){
        //If there is room in the node, insert the key/value pair 
        if(!node->isFull()){
            node->keyValues.insert(pair<int, string>(key, value));
            return true;
        }

        //Otherwise, if the leaf should be split among children
        else if(node->isLeaf && (node->parent == nullptr || node->parent->isFull())){
            //Split the node
            map<int, string>::iterator middlePair = splitNode(node);
            Node* leftChild = node->children[0];
            Node* rightChild = node->children[1];

            //Recursively inserts the key into the children
            if(key < middlePair->first){
                return handleNodeOverflow(leftChild, key, value);
            }
            else{
                return handleNodeOverflow(rightChild, key, value);
            }

            //Once split, check if the parent is also full
            if(node->parent->isFull()){
                //If so, recursively propogate the middle pair to the parent
                return handleNodeOverflow(node->parent, middlePair->first, middlePair->second);
            }   
        }
        //Otherwise,
        else{
            //Insert the value into the parent
            handleNodeOverflow(node->parent, key, value);

            //Split the child
            Node* newSibling = new Node(node->parent, true, this);
            allNodes.push_back(newSibling);
            node->keyValues.insert(pair<int, string>(key, value));

            //Balance values with the new sibling
            vector<int> keysToRemove;
            int counter = 0;
            for(map<int, string>::iterator it=node->keyValues.begin(); it!=node->keyValues.end(); it++){
                if(counter > floor((maxNumPointers-1)/2)){
                    keysToRemove.push_back(it->first);
                    newSibling->keyValues.insert(pair<int, string>(it->first, it->second));
                }
                counter++;
            }

            for(int i=0; i<keysToRemove.size(); i++){
                node->keyValues.erase(keysToRemove[i]);
            }

            //Blindly add the new sibling to the parent's children
            node->parent->children.push_back(newSibling);       

            //Sort the parent's children based on the first key in each child
            sort(node->parent->children.begin(), node->parent->children.end(), compareNodes);

            return true;
        }
    }
    return false;       //If the ndoes does not exist, it cannot be inserted into
}

bool BPlusTree::insertInternal(Node* node, int key, string value){
    //If the node is a leaf, simply call our recursiveInsertion method
    if(node->children.empty()){
        return handleNodeOverflow(node, key, value);
    }
    //Otherwise,
    else{
        //Check if there is a child which the key should go in
        int counter =  0;
        for(map<int, string>::iterator it = node->keyValues.begin(); it != node->keyValues.end(); it++){
            //If so, recursively call insertInteral to insert the value into the child
            if(key < it->first){
                return insertInternal(node->children[counter], key, value);
            }
            counter++;
        }

        //If not, check if there is room in the node
        if(node->isFull()){
            return handleNodeOverflow(node, key, value);       //If there is no room in the node, handle the overflow
        }
        else{
            return insertInternal(node->children.back(), key, value);      //If there is room, insert into the end pointer of the node
        }
    }
    return false;
}

//Each interior/root node must contain floor(maxNumPointers/2) pointers. => floor(maxNumPointers-1/2) values
bool BPlusTree::insert(int key, string value){
    //TODO: check for duplicates first!!
    //TODO: make sure leaf nodes point to each other!

    if(root == nullptr){   //If the root does not exist, enter the value into it
        root = new Node(nullptr, true, this);
        allNodes.push_back(root);
    }

    return insertInternal(root, key, value);;
}  

void BPlusTree::printKeys(){
    queue<Node*> nodesToPrint;
    nodesToPrint.push(root);

    //Iterate over the nodesToPrint queue
    while(!nodesToPrint.empty()){
        int sizeOfCurrentLevel = nodesToPrint.size();

        //Print the current level
        for(int i=0; i<sizeOfCurrentLevel; i++){
            Node* currentNode =  nodesToPrint.front();
            nodesToPrint.pop();
            printNodeKey(currentNode);

            //Add the children of the current node to the queue
            if(!currentNode->children.empty()){
                for(Node* child : currentNode->children){
                    nodesToPrint.push(child);
                }
            }
        }
        cout<<endl<<endl;
    }
    cout<<"--------------------------------\n";
}

void BPlusTree::printNodeKey(Node* node){
    cout<<"|";
    for(map<int, string>::iterator it = node->keyValues.begin(); it != node->keyValues.end(); it++){
        cout<<it->first;
        if(next(it) != node->keyValues.end()){
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
    tree->insert(23, "23");
    tree->printKeys();
    tree->insert(6, "6");
    tree->printKeys();
    tree->insert(19, "19");
    tree->insert(9, "9");
    tree->printKeys();
    tree->insert(7, "7");   //Works until here
    tree->printKeys();
}

