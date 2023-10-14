#include "BPlusTree.h"
#include <math.h>
#include <iostream>
#include <queue>

Node::Node(int n, bool isLeaf){
    keys = new int[n];
    children = new Node*[n+1];
    values = new string[n];
    size = 0;
    this->isLeaf = isLeaf;
}

BPlusTree::BPlusTree(int n){
    maxNumKeys = n;
    isEmpty = true;
}

BPlusTree::BPlusTree(BPlusTree& treeToCopy){
    maxNumKeys = treeToCopy.maxNumKeys;
    isEmpty = treeToCopy.isEmpty;
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

    return newNode;
}

BPlusTree& BPlusTree::operator=(BPlusTree& treeToCopy){
    if(this != &treeToCopy){
        this->~BPlusTree();
        this->maxNumKeys = treeToCopy.maxNumKeys;
        isEmpty = treeToCopy.isEmpty;
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
            insertionNode->size++;
            return true;
        }
    }

    //If there is no room in the insertion node, split it
    else{
        return splitNodeInsert(insertionNode, key, value, child);
    }
    return false;
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
    leftNode->size = 0;
    rightNode->size = 0;
    int j=0;
    if(leftNode->isLeaf){
        for(int i=0; i<maxNumKeys+1; i++){
            if(i > ceil((maxNumKeys+1)/2)){
                leftNodeKeys[i] = overflowKeys[i];
                leftNodeValues[i] = overflowValues[i];
                leftNode->size++;
            }
            else{
                rightNodeKeys[j] = overflowKeys[i];
                rightNodeValues[j] = overflowValues[i];
                rightNode->size++;
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
                if(!recursiveInsert(leftNode->parent, overflowKeys[i], "", rightNode)){
                    return false;
                }
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
    return true;
}

bool BPlusTree::insert(int key, string value){
    if(isEmpty){
        Node* root = new Node(maxNumKeys, true);
        root->keys[0] = key;
        root->values[0] = value;
        root->size++;
        isEmpty = false;
        return true;
    }

    Node* leafToInsert = findLeaf(key);
    return recursiveInsert(leafToInsert, key, value, nullptr);
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
            if(!currentNode->isLeaf){
                for(int i=0; i<currentNode->size+1; i++){
                    nodesToPrint.push(currentNode->children[i]);
                }
            }
        }
        cout<<endl<<endl;
    }
    cout<<"--------------------------------\n";
}

void BPlusTree::printNodeKey(Node* node){
    cout<<"|";
    for(int i=0; i<node->size; i++){
        cout<<node->keys[i];
        if(i != node->size-1){
            cout<<" ";
        }
    }
    cout<<"|";
}

void BPlusTree::printValues(){
    queue<Node*> nodesToPrint;
    nodesToPrint.push(root);

    //Iterate over the nodesToPrint queue
    while(!nodesToPrint.empty()){
        int sizeOfCurrentLevel = nodesToPrint.size();

        //Print the current level
        for(int i=0; i<sizeOfCurrentLevel; i++){
            Node* currentNode =  nodesToPrint.front();
            nodesToPrint.pop();
            printNodeValue(currentNode);

            //Add the children of the current node to the queue
            if(!currentNode->isLeaf){
                for(int i=0; i<currentNode->size+1; i++){
                    nodesToPrint.push(currentNode->children[i]);
                }
            }
        }
        cout<<endl<<endl;
    }
    cout<<"--------------------------------\n";
}

void BPlusTree::printNodeValue(Node* node){
    cout<<"|";
    for(int i=0; i<node->size; i++){
        cout<<node->values[i];
        if(i != node->size-1){
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
}