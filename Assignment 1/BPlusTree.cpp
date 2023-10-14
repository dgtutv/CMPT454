#include <BPlusTree.h>

Node::Node(int n){
    keys = new int[n];
    children = new Node*[n+1];
    values = new string[n];
}

BPlusTree::BPlusTree(int n){
    maxNumKeys = n+1;
}

BPlusTree::BPlusTree(BPlusTree& treeToCopy){
    maxNumKeys = treeToCopy.maxNumKeys;
    root = copyNode(treeToCopy.root);
}

Node* BPlusTree::copyNode(Node* nodeToCopy){
    Node* newNode = new Node(maxNumKeys-1);
    for(int i=0; i<)
}

BPlusTree& BPlusTree::operator=(BPlusTree& treeToCopy){
    if(this != &treeToCopy){
        this->~BPlusTree();
        this->maxNumKeys = treeToCopy.maxNumKeys;
        this->root = copyNode(treeToCopy.root);
    }
    return *this;
}