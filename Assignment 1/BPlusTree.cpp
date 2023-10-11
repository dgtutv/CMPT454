#include "BPlusTree.h"
#include <algorithm>
#include <math.h>
#include <iostream>
using namespace std;

/*------------------------------------------Node----------------------------------------*/
Node::Node(Node* parent, bool isLeaf) : parent(parent), isLeaf(isLeaf){};

/*----------------------------------------BPlusTree-------------------------------------*/
BPlusTree::BPlusTree(int maxNumPointers) : maxNumPointers(maxNumPointers){};

//Each interior/root node must contain floor(maxNumPointers/2) pointers. => floor(maxNumPointers-1/2) values
bool BPlusTree::insert(int key, string value){
    //TODO: check for duplicates first!!

    if(root == NULL){   //If the root does not exist, enter the value into it
        root = new Node(NULL, true);
        root->keyValues.insert(pair<int, string>(key, value));
    }
    else if(root->children.size() == 0){    //Check if the root has no children
        if(root->keyValues.size() < maxNumPointers-1){      //If the root is not full, insert into the root
            root->keyValues.insert(pair<int, string>(key, value));
        }
        else{   //If the root is full
            root->isLeaf = false;
            Node* leftChild = new Node(root, true);
            Node* rightChild = new Node(root, true);
            root->children.push_back(leftChild);
            root->children.push_back(rightChild);
            map<int, string>::iterator middlePair;
            int counter = 0;

            //Distribute the key/values
            for(map<int, string>::iterator it = root->keyValues.begin(); it != root->keyValues.end(); it++){  
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
            for (map<int, string>::iterator it = root->keyValues.begin(); it != root->keyValues.end();) {
                if(it != middlePair){
                    root->keyValues.erase(it++);
                } 
                else{
                    ++it;
                }
            }

            //Add the new key/value pair
            if(key < middlePair->first){
                leftChild->keyValues.insert(pair<int, string>(key, value));
            }
            else{
                rightChild->keyValues.insert(pair<int, string>(key, value));
            }
        }
    }
    return false;
}  

void BPlusTree::printKeys(){
    printNodeKey(root);
    cout<<endl;
    for(list<Node*>::iterator it = root->children.begin(); it != root->children.end(); it++){
        printNodeKey(*it);
    }
    cout<<endl;
}

void BPlusTree::printNodeKey(Node* node){
    cout<<"|";
    for(map<int, string>::iterator it = node->keyValues.begin(); it != node->keyValues.end(); it++){
        cout<<it->first<<" ";
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
}

