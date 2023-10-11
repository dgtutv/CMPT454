#include "BPlusTree.h"
#include <algorithm>
#include <math.h>
using namespace std;

/*------------------------------------------Node----------------------------------------*/
Node::Node(Node* parent, bool isLeaf) : parent(parent), isLeaf(isLeaf){};

/*----------------------------------------BPlusTree-------------------------------------*/
BPlusTree::BPlusTree(int maxNumPointers) : maxNumPointers(maxNumPointers){};

//Each interior/root node must contain floor(maxNumPointers/2) pointers. => floor(maxNumPointers-1/2) values
bool BPlusTree::insert(int key, string value, Node* localRoot){
    if(root == NULL){   //If the root does not exist, enter the value into it
        root = new Node(NULL, true);
        root->keyValues.insert(pair<int, string>(key, value));
    }
    else if(root->keyValues.size() < floor((maxNumPointers-1)/2)){      //If the root has less than half full, insert into the root 
        root->keyValues.insert(pair<int, string>(key, value));
    }
}     