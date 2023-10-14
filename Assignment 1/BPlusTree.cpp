#include "BPlusTree.h"

Node::Node(int n, bool isLeaf){
    keys = new int[n];
    children = new Node*[n+1];
    values = new string[n];
    size = 0;
    this->isLeaf = isLeaf;
}

BPlusTree::BPlusTree(int n){
    maxNumKeys = n+1;
}

BPlusTree::BPlusTree(BPlusTree& treeToCopy){
    maxNumKeys = treeToCopy.maxNumKeys;
    root = copyNode(treeToCopy.root);
}

Node* BPlusTree::copyNode(Node* nodeToCopy){
    Node* newNode = new Node(maxNumKeys-1, nodeToCopy->isLeaf);
    newNode->size = nodeToCopy->size;
    newNode->parent = nodeToCopy->parent;
    for(int i=0; i<newNode->size; i++){
        newNode->keys[i] = nodeToCopy->keys[i];
        newNode->values[i] = nodeToCopy->values[i];
    }
    if(!newNode->isLeaf){
        for(int i=0; i<newNode->size+1; i++){
            newNode->children[i] = nodeToCopy->children[i];
        }
    }
}

BPlusTree& BPlusTree::operator=(BPlusTree& treeToCopy){
    if(this != &treeToCopy){
        this->~BPlusTree();
        this->maxNumKeys = treeToCopy.maxNumKeys;
        this->root = copyNode(treeToCopy.root);
    }
    return *this;
}