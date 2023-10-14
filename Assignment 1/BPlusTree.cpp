#include "BPlusTree.h"
#include <algorithm>
#include <math.h>
#include <iostream>
#include <queue>
using namespace std;

/*------------------------------Compare function for nodes-------------------------------*/
bool compareNodes(const BPlusTree::Node* a, const BPlusTree::Node* b){
}

/*------------------------------------------Node----------------------------------------*/
BPlusTree::Node::Node(Node* parent, bool isLeaf, BPlusTree* tree) : parent(parent), isLeaf(isLeaf), tree(tree){};

bool BPlusTree::Node::isFull() const{
}

BPlusTree::Node::~Node(){}  //Data is redirected outside of the class, not deleted

/*----------------------------------------BPlusTree-------------------------------------*/
BPlusTree::BPlusTree(int maxNumPointers) : maxNumPointers(maxNumPointers){};

void BPlusTree::splitNode(Node* leftNode, int key, string value){
}

BPlusTree::Node* BPlusTree::findNode(int key){
}

string BPlusTree::find(int key){
}

bool BPlusTree::insert(int key, string value){
    //If the root does not exist, enter the key/value into it 
    if(root == nullptr){
        root = new Node(nullptr, true, this);
        allNodes.push_back(root);
        pair<int, void*> keyValue = pair<int, void*>(key, (void*)&value);
        root->keyPointers.insert(keyValue);
    }

    //Check if the key is already present
    if(find(key).size() != 0){
        return false;
    }

    //Otherwise, find where to insert our pair
    else{
        Node* insertionNode = findNode(key);
        if(insertionNode->isFull()){
            splitNode(insertionNode, key, value);
        }
        else{
            pair<int, void*> keyValue = pair<int, void*>(key, (void*)&value);
            insertionNode->keyPointers.insert(keyValue);
        }
    }
    return true;
}  

int BPlusTree::findIndexOfNodeInParent(Node* child){
}

int BPlusTree::findAssociatedKeyOfNodeInParent(Node* child){
}

void BPlusTree::redistribute(Node* victim, Node* receiver, bool victimLeftOfReceiver){
}

BPlusTree::Node* BPlusTree::findLeafToLeftOfNode(Node* node){
}

void BPlusTree::coalesce(Node* victim, Node* receiver, bool victimLeftOfReceiver){
}

void BPlusTree::removeFromNode(Node* node, int key){
}

bool BPlusTree::remove(int key){
}

void BPlusTree::printKeys(){
    queue<Node*> nodesToPrint;
    nodesToPrint.push(root);

    //Iterate over the nodesToPrint queue
    while(!nodesToPrint.empty()){
        int sizeOfCurrentLevel = nodesToPrint.size();

        //Print the current level
        for(int i=0; i<sizeOfCurrentLevel; i++){
            Node* currentNode = nodesToPrint.front();
            nodesToPrint.pop();
            printNodeKey(currentNode);

            //Add the children of the currentNode to the queue
            if(!currentNode->isLeaf){
                for(auto it = currentNode->keyPointers.begin(); it != currentNode->keyPointers.end(); it++){
                    nodesToPrint.push((Node*)it->second);
                }
            }
        }
        cout<<endl<<endl; 
    } 
    cout<<"--------------------------------\n"; 
}

void BPlusTree::printNodeKey(Node* node){
    cout<<"|";
    for(auto it = node->keyPointers.begin(); it != node->keyPointers.end(); it++){
        cout<<it->first;
        if(next(it) != node->keyPointers.end()){
            cout<<" ";
        }
    }
    cout<<"|";
}

//Tester code
int main(int argc, char const *argv[]){
    BPlusTree* tree = new BPlusTree(4);
    tree->insert(21, "twenty one");
    tree->insert(2, "two");
    tree->insert(11, "eleven");
    tree->insert(8, "eight");
    tree->insert(64, "sixty four");
    tree->insert(5, "five");  
    tree->insert(23, "twenty three");
    tree->insert(6, "six");
    tree->insert(19, "nineteen");
    tree->insert(9, "nine");
    tree->insert(7, "seven");   
    tree->insert(31, "thirty one");  
    tree->insert(45, "fourty five");
    tree->insert(39, "thirty nine");    
    tree->insert(60, "sixty");   
    tree->insert(51, "fifty one");
    tree->insert(97, "ninety seven");
    tree->insert(77, "seventy seven");  

    tree->printKeys();

    // tree->remove(45);   //Taking from wrong child, not updating parent pointer, also parent pointer still broken, but worked around it
    // tree->printKeys();
}
