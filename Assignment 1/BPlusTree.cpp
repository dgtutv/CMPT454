#include "BPlusTree.h"

Node::Node(int n, bool isLeaf){
    keys = new int[n];
    children = new Node*[n+1];
    values = new string[n];
    size = 0;
    this->isLeaf = isLeaf;
}

BPlusTree::BPlusTree(int n){
    maxNumKeys = n;
}

BPlusTree::BPlusTree(BPlusTree& treeToCopy){
    maxNumKeys = treeToCopy.maxNumKeys;
    root = copyNode(treeToCopy.root);
}

Node* BPlusTree::copyNode(Node* nodeToCopy){
    Node* newNode = new Node(maxNumKeys, nodeToCopy->isLeaf);
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

void BPlusTree::recursiveDelete(Node* node){
    if(!node->isLeaf){
        for(int i=0; i<node->size+1; i++){
            recursiveDelete(node->children[i]);
        }
    }
    delete node;

}

BPlusTree::~BPlusTree(){
    recursiveDelete(root);
}

//Returs the leaf node the key is already in, or the leaf node of where the key should be inserted
Node* BPlusTree::findLeaf(int key){
    Node* currentNode = root;
    int currentKey;
    while(!currentNode->isLeaf){
        for(int i=0; i<currentNode->size; i++){
            currentKey = currentNode->keys[i];
            if(key < currentKey){
                currentNode = currentNode->children[i];
            }
            else if(key > currentKey && i!=currentNode->size-1){
                continue;
            }
            else{
                currentNode=currentNode->children[i+1];
            }
        }
    }
    return(currentNode);
}

bool BPlusTree::recursiveInsert(Node* insertionNode, int key, string value, Node* child){
    //If there is room in the insertionNode
    if(insertionNode->size < maxNumKeys){
        //Find where to insert the key (and possibly value)
        for(int i=0; i<insertionNode->size; i++){
            //If needed, shove values right 
            if(key < insertionNode->keys[i]){
                for(int j=insertionNode->size; j>=i; j--){
                    insertionNode->keys[j] = insertionNode->keys[j-1];
                    if(insertionNode->isLeaf){
                        insertionNode->values[j] = insertionNode->values[j-1];
                    }
                    else{
                        insertionNode->children[j+1] = insertionNode->children[j];
                    }
                }
                
            }

            //Insert the key
            insertionNode->keys[i] = key;
            if(insertionNode->isLeaf){
                insertionNode->values[i] = value;
            }
            else{
                insertionNode->children[i+1] = child;
            }
            return true;
        }
    }

    //If there is no room in the insertion node, split it
    else{
        splitNode(insertionNode);
        return(recursiveInsert(insertionNode, key, value, child));
    }
}

bool BPlusTree::insert(int key, string value){
    Node* leafToInsert = findLeaf(key);
    return recursiveInsert(leafToInsert, key, value, nullptr);
}