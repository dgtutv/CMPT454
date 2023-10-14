#include "BPlusTree.h"
#include <cmath>
#include <iostream>>

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
        //Find where to insert the elements
        for(int i=0; i<insertionNode->size; i++){
            //If needed, shove elements right 
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

            //Insert the elements
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
        return splitNodeInsert(insertionNode, key, value, child);
    }
}

bool BPlusTree::splitNodeInsert(Node* leftNode, int key, string value, Node* child){
    Node* rightNode = new Node(maxNumKeys, rightNode->isLeaf);
    rightNode->parent = leftNode->parent;

    //Duplicate our elements into an array one larger
    int overflowKeys[maxNumKeys+1];
    string overflowValues[maxNumKeys+1];
    Node* overflowChildren[maxNumKeys+2];
    for(int i=0; i<maxNumKeys; i++){
        overflowKeys[i] = leftNode->keys[i];
        if(leftNode->isLeaf){
            overflowValues[i] = leftNode->values[i];
        }
    }
    if(!leftNode->isLeaf){
        for(int i=0; i<maxNumKeys+1; i++){
            overflowChildren[i] = leftNode->children[i];
        }
    }

    //Find where to insert the elements
    for(int i=0; i<maxNumKeys+1; i++){
        //If needed, shove elements right 
        if(key < overflowKeys[i]){
            for(int j=maxNumKeys+1; j>=i; j--){
                overflowKeys[j] = overflowKeys[j-1];
                if(leftNode->isLeaf){
                    overflowValues[j] = overflowValues[j-1];
                }
                else{
                    overflowChildren[j+1] = overflowChildren[j];
                }
            } 
        }

        //Insert the elements
        overflowKeys[i] = key;
        if(leftNode->isLeaf){
            overflowValues[i] = value;
        }
        else{
            overflowChildren[i+1] = child;
        }
    }

    //Iterate over our overflow arrays, and balance nodes
    int leftNodeKeys[maxNumKeys];
    string leftNodeValues[maxNumKeys];
    Node* leftNodeChildren[maxNumKeys+1];
    int rightNodeKeys[maxNumKeys];
    string rightNodeValues[maxNumKeys];
    Node* rightNodeChildren[maxNumKeys+1];
    int j=0;
    if(leftNode->isLeaf){
        for(int i=0; i<maxNumKeys+1; i++){
            if(i > ceil((maxNumKeys+1)/2)){
                leftNodeKeys[i] = overflowKeys[i];
                leftNodeValues[i] = overflowValues[i];
            }
            else{
                rightNodeKeys[j] = overflowKeys[i];
                rightNodeValues[j] = overflowValues[i];
                j++;
            }
        }
    }
    else{
        int j=0;
        for(int i=0; i<maxNumKeys+1; i++){
            if(i < ceil(maxNumKeys/2)){
                leftNodeKeys[i] = overflowKeys[i];
            }
            else if(i == ceil(maxNumKeys/2)){
                recursiveInsert(leftNode->parent, overflowKeys[i], "", rightNode);
            }
            else{
                rightNodeKeys[j] = overflowKeys[i];
                j++;
            }
        }
        j=0;
        for(int i=0; i<maxNumKeys+2; i++){
            if(i < ceil((maxNumKeys+2)/2)){
                leftNodeChildren[i] = overflowChildren[i];
            }
            else{
                rightNodeChildren[j] = overflowChildren[i];
                j++;
            }
        }
    }
    leftNode->keys = leftNodeKeys;
    leftNode->values = leftNodeValues;
    leftNode->children = leftNodeChildren;
    rightNode->keys = rightNodeKeys;
    rightNode->values = rightNodeValues;
    rightNode->children = rightNodeChildren; 

}

bool BPlusTree::insert(int key, string value){
    Node* leafToInsert = findLeaf(key);
    return recursiveInsert(leafToInsert, key, value, nullptr);
}