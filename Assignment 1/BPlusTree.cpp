#include "BPlusTree.h"
#include <algorithm>
#include <math.h>
using namespace std;

/*------------------------------------------Node----------------------------------------*/
Node::Node(Node* parent, bool isLeaf) : parent(parent), isLeaf(isLeaf){};

/*----------------------------------------BPlusTree-------------------------------------*/
BPlusTree::BPlusTree(int maxNumPointers) : maxNumPointers(maxNumPointers){};

//Each interior/root node must contain floor(maxNumPointers/2) pointers. => floor(maxNumPointers-1/2) values
bool BPlusTree::insert(int key, string value){
    if(root == NULL){   //If the root does not exist, enter the value into it
        root = new Node(NULL, true);
        root->keyValues.insert(pair<int, string>(key, value));
    }
    else if(root->children.size() == 0){    //Check if the root has children
        if(root->keyValues.size() < maxNumPointers-1){      //If the root is not full, insert into the root
            root->keyValues.insert(pair<int, string>(key, value));
        }
        else{   //If the root is full, distribute among two chldren
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
                if(counter == floor((maxNumPointers-1)/2)){middlePair = it;}
                else{
                    rightChild->keyValues[it->first] = it->second;
                }
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
        }
    }
}     
