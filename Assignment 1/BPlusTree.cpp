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
    return keyValues.size() > tree->maxNumPointers-1;
}

bool BPlusTree::Node::isFull() const{
    return keyValues.size() >= tree->maxNumPointers-1;
}

/*----------------------------------------BPlusTree-------------------------------------*/
BPlusTree::BPlusTree(int maxNumPointers) : maxNumPointers(maxNumPointers){};

void BPlusTree::updateRoot(Node* newRoot){
    root = newRoot;
}

void BPlusTree::splitNode(Node* leftNode){
    //Create a sibling node
    Node* rightNode = new Node(leftNode->parent, leftNode->isLeaf, this);
    int counter = 0;
    vector<int> keysToRemove;
    vector<Node*> pointersToRemove;
    
    //Balance the siblings
    for(map<int, string>::iterator it = rightNode->keyValues.begin(); it != rightNode->keyValues.end(); it++){
        if(counter > floor((maxNumPointers-1)/2)){
            rightNode->keyValues.insert(pair<int, string>(it->first, it->second));
            keysToRemove.push_back(it->first);
            if(!leftNode->isLeaf){
                rightNode->children.push_back(leftNode->children[counter]);
                pointersToRemove.push_back(leftNode->children[counter]);
            }
        }
        counter++;
    }

    //Remove extraneous data from the leftNode
    for(int i=0; i<keysToRemove.size(); i++){
        leftNode->keyValues.erase(keysToRemove[i]);
        if(!leftNode->isLeaf){
            vector<Node*>::iterator it = std::find(leftNode->children.begin(), leftNode->children.end(), pointersToRemove[i]);
            leftNode->children.erase(it);
        }
    }

    //If a parent does not exist, make an empty one
    if(leftNode == root){
        Node* newParent = new Node(nullptr, false, this);
        leftNode->parent = newParent;
        rightNode->parent = newParent;
        leftNode->isLeaf = false;
        rightNode->isLeaf = false;
    }

    //If there is a parent, but it is full, recursively call splitNode on the parent
    else if(leftNode->parent->isFull()){
        splitNode(leftNode->parent);
    }

    //Insert rightNode's first key into the parent
    leftNode->parent->keyValues.insert(pair<int, string>(rightNode->keyValues.begin()->first, rightNode->keyValues.begin()->second));

    //Override pointers adjacent to the newly inserted key to point at the leftNode and the rightNode
    leftNode->parent->children.push_back(rightNode);
    sort(leftNode->parent->children.begin(), leftNode->parent->children.end(), compareNodes);

    //If the rightNode is not a leaf, and it has more than one key, remove rightNode's first value
    if(!(rightNode->isLeaf) && (rightNode->keyValues.size() > 1)){
        rightNode->keyValues.erase(rightNode->keyValues.begin()->first);
    }

}

//Each interior/root node must contain floor(maxNumPointers/2) pointers. => floor(maxNumPointers-1/2) values
bool BPlusTree::insert(int key, string value){
    //TODO: check for duplicates first!!
    //TODO: make sure leaf nodes point to each other!

    if(root == nullptr){   //If the root does not exist, enter the value into it
        root = new Node(nullptr, true, this);
        allNodes.push_back(root);
    }

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

