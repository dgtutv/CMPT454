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
    return keyPointers.size() >= tree->maxNumPointers-1;
}

BPlusTree::Node::~Node(){}  //Data is redirected outside of the class, not deleted

/*----------------------------------------BPlusTree-------------------------------------*/
BPlusTree::BPlusTree(int maxNumPointers) : maxNumPointers(maxNumPointers){};

void BPlusTree::splitNode(Node* leftNode, int key, void* pointer){
    //Create a sibling node
    Node* rightNode = new Node(leftNode->parent, leftNode->isLeaf, this);
    allNodes.push_back(rightNode);

    //Insert the new key and pointer into the left node
    leftNode->keyPointers.insert(pair<int, void*>(key, pointer));

    //Balance the siblings
    int counter = 0;
    vector<int> keysToRemove;
    for(auto it = leftNode->keyPointers.begin(); it != leftNode->keyPointers.end(); it++){
        if(counter >= floor(maxNumPointers/2)){
            rightNode->keyPointers.insert(pair<int, void*>(key, pointer));
            keysToRemove.push_back(it->first);
        }
        counter++;
    }

    //Remove extraneous data from the left node
    for(int i=0; i<keysToRemove.size(); i++){
        leftNode->keyPointers.erase(keysToRemove[i]);
    }

    //If a parent does not exist, make one
    if(leftNode == root){
        Node* newParent = new Node(nullptr, false, this);
        allNodes.push_back(newParent);
        leftNode->parent = newParent;
        newParent->firstChild = leftNode;
        root = newParent;
        leftNode->parent->keyPointers.insert(pair<int, void*>(rightNode->keyPointers.begin()->first, (void*)rightNode));
    }

    //If there is a parent, but it is full, recursively call splitNode on the parent
    else if(leftNode->parent->isFull()){
        splitNode(leftNode->parent, rightNode->keyPointers.begin()->first, rightNode);
    }

    //Otherwise, insert rightNode's first key, and a pointer to rightNode into the parent
    else{
        leftNode->parent->keyPointers.insert(pair<int, void*>(rightNode->keyPointers.begin()->first, (void*)rightNode));
    }

    //Set rightNode's (the new sibling's) parent
    rightNode->parent = leftNode->parent;

    //If rightNode is not a leaf, and it has more than one key, remove rightNode's first keyPointer pair
    if(!(rightNode->isLeaf) && (rightNode->keyPointers.size() > 1)){
        rightNode->keyPointers.erase(rightNode->keyPointers.begin()->first);
    }
}

BPlusTree::Node* BPlusTree::findNode(int key){
    Node* currentNode = root;

    while(!currentNode->isLeaf){
        //If the key is less than the first key in the current node
        if(key < currentNode->keyPointers.begin()->first){
            //See the first child
            currentNode = currentNode->firstChild;
        }

        //If the key is greater than or equal to the largest key in the current node
        else if(key >= currentNode->keyPointers.rbegin()->first){
            //See the last child
            currentNode = (Node*)currentNode->keyPointers.rbegin()->second;
        }

        //Otherwise, find the corresponding child with the correct key
        else{
            //Catch a bad call
            if(currentNode->keyPointers.size() < 2){
                return nullptr;
            }

            //Iterate over our keyPointers map
            int currKey;
            for(auto it = next(currentNode->keyPointers.begin()); it != currentNode->keyPointers.end(); it++){
                currKey = it->first;
                //If we find our key to be less than the current key, go to the previous child
                if(key<currKey){
                    currentNode = (Node*)(prev(it)->second);
                    break;
                }
                //If we find our key to be the same as the current key, go to the current child
                else if(key == currKey){
                    currentNode = (Node*)(it->second);
                    break;
                }
            }
        }
    }
    return currentNode;
}

string BPlusTree::find(int key){
    //Find the leaf that the key is in
    Node* node = findNode(key);

    //If the key was not found, return an empty string
    if(node == nullptr){
        return "";
    }

    //Otherwise, return the value corresponding with the key in leaf
    return *(string*)node->keyPointers.find(key)->second;
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
            splitNode(insertionNode, key, (void*)&value);
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
