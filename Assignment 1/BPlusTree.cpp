#include "BPlusTree.h"
#include <algorithm>
using namespace std;

/*------------------------------------------Node----------------------------------------*/
Node::Node(Node* parent, bool isLeaf) : parent(parent), isLeaf(isLeaf){};

/*----------------------------------------BPlusTree-------------------------------------*/
BPlusTree::BPlusTree(int maxNumPointers) : maxNumPointers(maxNumPointers){};

bool BPlusTree::insert(int key, string value){
    
}