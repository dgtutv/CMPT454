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

void BPlusTree::splitNode(Node* leftNode, int key, string value){
    //Create a sibling node
    Node* rightNode = new Node(leftNode->parent, leftNode->isLeaf, this);
    allNodes.push_back(rightNode);
    int counter = 0;
    vector<int> keysToRemove;
    vector<Node*> pointersToRemove;

    //Insert the new key/value into the leftNode
    leftNode->keyValues.insert(pair<int, string>(key, value));
    
    //Balance the siblings
    for(map<int, string>::iterator it = leftNode->keyValues.begin(); it != leftNode->keyValues.end(); it++){
        if(counter >= floor(leftNode->keyValues.size()/2)){
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
    sort(leftNode->children.begin(), leftNode->children.end(), compareNodes);

    //If a parent does not exist, make an empty one
    if(leftNode == root){
        Node* newParent = new Node(nullptr, false, this);
        allNodes.push_back(newParent);
        leftNode->parent = newParent;
        rightNode->parent = newParent;
        newParent->children.push_back(leftNode);
        root = newParent;
    }

    //Update leftNode and rightNode's isLeaf booleans
    if(leftNode->children.size() == 0){
        leftNode->isLeaf = true;
        rightNode->isLeaf = true;
        if(leftNode->nextLeaf != nullptr){
            rightNode->nextLeaf = leftNode->nextLeaf;
        }
        leftNode->nextLeaf = rightNode;
    }

    //If there is a parent, but it is full, recursively call splitNode on the parent
    if(leftNode->parent->isFull()){
        splitNode(leftNode->parent, rightNode->keyValues.begin()->first, rightNode->keyValues.begin()->second);
        sort(leftNode->parent->children.begin(), leftNode->parent->children.end(), compareNodes);
    }

    //Insert rightNode's first key into the parent
    else{
        leftNode->parent->keyValues.insert(pair<int, string>(rightNode->keyValues.begin()->first, rightNode->keyValues.begin()->second));
    }
    //Override pointers adjacent to the newly inserted key to point at the leftNode and the rightNode
    leftNode->parent->children.push_back(rightNode);
    sort(leftNode->parent->children.begin(), leftNode->parent->children.end(), compareNodes);

    //If the rightNode is not a leaf, and it has more than one key, remove rightNode's first value
    if(!(rightNode->isLeaf) && (rightNode->keyValues.size() > 1)){
        rightNode->keyValues.erase(rightNode->keyValues.begin()->first);
    }

}

BPlusTree::Node* BPlusTree::findNode(int key){
    Node* currentNode = root;

    while(!currentNode->isLeaf){
        //If the key is less than the first key in the current node
        if(key < currentNode->keyValues.begin()->first){
            //See the first child
            currentNode = *currentNode->children.begin();
            continue;
        }

        //If the key is greater than or equal to the largest key in the current node
        else if(key >= currentNode->keyValues.rbegin()->first){
            //See the last child
            currentNode = *currentNode->children.rbegin();
            continue;
        }

        //Otherwise, find the corresponding child with the correct key
        if(currentNode->keyValues.size()<2){
            return nullptr;
        }
        //Iterate over our keyValues map 
        int currKey;
        int i=0;
        for(map<int, string>::iterator it=currentNode->keyValues.begin(); it != currentNode->keyValues.end(); ++it){
            currKey = it->first;
            //If we find our key to be less than the current key, go to the ith child
            if(key<currKey){
                currentNode = currentNode->children[i];
                break;
            }
            //If we find our key to be the same as the current key, go to the i+1th child
            else if(key==currKey){
                currentNode = currentNode->children[i+1];
                break;
            }
            ++i;
        }
    }
    return currentNode;
}

string BPlusTree::find(int key){
    //Find the node that the key is in
    Node* node = findNode(key);
    //If the key was not found, return null
    if(node == nullptr){
        return nullptr;
    }
    //Otherwise, return the value corresponding with the key in the node
    return node->keyValues.find(key)->second;
}

//Each interior/root node must contain floor(maxNumPointers/2) pointers. => floor(maxNumPointers-1/2) values
bool BPlusTree::insert(int key, string value){
    //TODO: check for duplicates first!! (do so by implementing find, and seeing if you get a value)

    //If the root does not exist, enter the value into it
    if(root == nullptr){   
        root = new Node(nullptr, true, this);
        allNodes.push_back(root);
        root->keyValues.insert(pair<int, string>(key, value));
    }

    //Otherwise, find where to insert our pair
    else{
        Node* insertionNode = findNode(key);
        if(insertionNode->isFull() || insertionNode->isOverflow()){
            splitNode(insertionNode, key, value);
        }
        else{
            insertionNode->keyValues.insert(pair<int, string>(key, value));
        }
    }
    return true;
}  

void BPlusTree::redistribute(Node* victim, Node* receiver){

}

void BPlusTree::coalesce(Node* victim, Node* receiver){

}

void BPlusTree::removeFromNode(Node* node, int key, Node* pointer){
    
}

bool BPlusTree::remove(int key){
    //Find the associated leaf node
    Node* leaf = findNode(key);

    if(leaf == nullptr){
        return false;
    }

    else{
        removeFromNode(leaf, key, nullptr);
    }
}

void BPlusTree::printLeaves(){
    Node* currNode = root;
    while(!currNode->isLeaf){
        currNode = *currNode->children.begin();
    }
    while(true){
        cout<<"{";
        int counter = 0;
        for(auto it=currNode->keyValues.begin(); it != currNode->keyValues.end(); it++){
            cout<<"("<<it->first<<", "<<it->second<<")";
            if(counter != currNode->keyValues.size()-1){
                cout<<", ";
            }
            counter++;
        }
        cout<<"}";
        currNode = currNode->nextLeaf;
        if(currNode == nullptr){
            cout<<endl;
            return;
        }
        else{
            cout<<" -> ";
        }
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
    tree->insert(21, "twenty one");
    tree->insert(2, "two");
    tree->insert(11, "eleven");
    cout<<"\nInsert 21, 2, 11\n";
    tree->printKeys();
    tree->printLeaves();
    // cout<<tree->find(21)<<", ";
    // cout<<tree->find(2)<<", ";
    // cout<<tree->find(11)<<", ";
    // cout<<tree->find(85)<<endl;
    tree->insert(8, "eight");
    cout<<"\nInsert 8\n";
    tree->printKeys();
    tree->printLeaves();
    // cout<<tree->find(21)<<", ";
    // cout<<tree->find(2)<<", ";
    // cout<<tree->find(11)<<", ";
    // cout<<tree->find(8)<<", ";
    // cout<<tree->find(85)<<endl;
    tree->insert(64, "sixty four");
    cout<<"\nInsert 64\n"; 
    tree->printKeys();
    tree->printLeaves();
    // cout<<tree->find(21)<<", ";
    // cout<<tree->find(2)<<", ";
    // cout<<tree->find(11)<<", ";
    // cout<<tree->find(8)<<", ";
    // cout<<tree->find(64)<<", ";
    // cout<<tree->find(85)<<endl;
    tree->insert(5, "five");
    cout<<"\nInsert 5\n";   
    tree->printKeys();
    tree->printLeaves();
    // cout<<tree->find(21)<<", ";
    // cout<<tree->find(2)<<", ";
    // cout<<tree->find(11)<<", ";
    // cout<<tree->find(8)<<", ";
    // cout<<tree->find(64)<<", ";
    // cout<<tree->find(5)<<", ";
    // cout<<tree->find(85)<<endl;
    tree->insert(23, "twenty three");
    cout<<"\nInsert 23\n";
    tree->printKeys();
    tree->printLeaves();
    // cout<<tree->find(21)<<", ";
    // cout<<tree->find(2)<<", ";
    // cout<<tree->find(11)<<", ";
    // cout<<tree->find(8)<<", ";
    // cout<<tree->find(64)<<", ";
    // cout<<tree->find(5)<<", ";
    // cout<<tree->find(23)<<", ";
    // cout<<tree->find(85)<<endl;
    tree->insert(6, "six");
    cout<<"\nInsert 6\n";
    tree->printKeys();
    tree->printLeaves(); 
    // cout<<tree->find(21)<<", ";
    // cout<<tree->find(2)<<", ";
    // cout<<tree->find(11)<<", "; 
    // cout<<tree->find(8)<<", ";
    // cout<<tree->find(64)<<", "; 
    // cout<<tree->find(5)<<", ";
    // cout<<tree->find(23)<<", ";
    // cout<<tree->find(6)<<", "; 
    // cout<<tree->find(85)<<endl;
    tree->insert(19, "nineteen");
    cout<<"\nInsert 19\n";
    tree->printKeys();
    tree->printLeaves();
    // cout<<tree->find(21)<<", "; 
    // cout<<tree->find(2)<<", ";
    // cout<<tree->find(11)<<", "; 
    // cout<<tree->find(8)<<", ";
    // cout<<tree->find(64)<<", ";
    // cout<<tree->find(5)<<", ";
    // cout<<tree->find(23)<<", ";
    // cout<<tree->find(6)<<", "; 
    // cout<<tree->find(19)<<", ";
    // cout<<tree->find(85)<<endl;
    tree->insert(9, "nine");
    cout<<"\nInsert 9\n";
    tree->printKeys();
    tree->printLeaves();
    // cout<<tree->find(21)<<", ";
    // cout<<tree->find(2)<<", ";
    // cout<<tree->find(11)<<", ";
    // cout<<tree->find(8)<<", ";
    // cout<<tree->find(64)<<", ";
    // cout<<tree->find(5)<<", ";
    // cout<<tree->find(23)<<", ";
    // cout<<tree->find(6)<<", ";
    // cout<<tree->find(19)<<", ";
    // cout<<tree->find(9)<<", ";
    // cout<<tree->find(85)<<endl;
    tree->insert(7, "seven");
    cout<<"\nInsert 7\n";
    tree->printKeys();
    tree->printLeaves();
    // cout<<tree->find(21)<<", ";
    // cout<<tree->find(2)<<", ";
    // cout<<tree->find(11)<<", ";
    // cout<<tree->find(8)<<", ";
    // cout<<tree->find(64)<<", ";
    // cout<<tree->find(5)<<", ";
    // cout<<tree->find(23)<<", ";
    // cout<<tree->find(6)<<", "; 
    // cout<<tree->find(19)<<", ";
    // cout<<tree->find(9)<<", ";
    // cout<<tree->find(7)<<", ";
    // cout<<tree->find(85)<<endl;
}

