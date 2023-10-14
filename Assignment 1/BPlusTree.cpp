#include "BPlusTree.h"
#include <algorithm>
#include <math.h>
#include <iostream>
#include <queue>
using namespace std;

/*------------------------------Compare function for nodes-------------------------------*/
bool compareNodes(const BPlusTree::Node* a, const BPlusTree::Node* b){
    if (!a->keyValues.empty() && !b->keyValues.empty()) {
        return a->keyValues.begin()->first < b->keyValues.begin()->first;
    }
    return false;       //Return false if one or both of the nodes has no keys
}

/*------------------------------------------Node----------------------------------------*/
BPlusTree::Node::Node(Node* parent, bool isLeaf, BPlusTree* tree) : parent(parent), isLeaf(isLeaf), tree(tree){};

bool BPlusTree::Node::isOverflow() const{
    return keyValues.size() > tree->maxNumPointers-1;
}

bool BPlusTree::Node::isFull() const{
    return keyValues.size() >= tree->maxNumPointers-1;
}

BPlusTree::Node::~Node(){}  //Data is redirected outside of the class, not deleted

/*----------------------------------------BPlusTree-------------------------------------*/
BPlusTree::BPlusTree(int maxNumPointers) : maxNumPointers(maxNumPointers){};

void BPlusTree::updateParentPointers(Node* parent){
    for(auto it=parent->children.begin(); it!=parent->children.end(); it++){
        (*it)->parent = parent;
        updateParentPointers(*it);
    }
}

void BPlusTree::splitNode(Node* leftNode, int key, string value){
    //Create a sibling node
    Node* rightNode = new Node(leftNode->parent, leftNode->isLeaf, this);
    allNodes.push_back(rightNode);
    int counter = 0;
    vector<int> keysToRemove;
    vector<Node*> pointersToRemove;

    //Insert the new key/value into the leftNode
    leftNode->keyValues.insert(pair<int, string>(key, value));
    
    //Balance the siblings
    for(map<int, string>::iterator it = leftNode->keyValues.begin(); it != leftNode->keyValues.end(); it++){
        if(counter >= floor(leftNode->keyValues.size()/2)){
            rightNode->keyValues.insert(pair<int, string>(it->first, it->second));
            keysToRemove.push_back(it->first);
            if(!leftNode->isLeaf){
                rightNode->children.push_back(leftNode->children[counter]);
                pointersToRemove.push_back(leftNode->children[counter]);
            }
        }
        counter++;
    }

    //Remove extraneous data from the leftNode
    for(int i=0; i<keysToRemove.size(); i++){
        leftNode->keyValues.erase(keysToRemove[i]);
        if(!leftNode->isLeaf){
            vector<Node*>::iterator it = std::find(leftNode->children.begin(), leftNode->children.end(), pointersToRemove[i]);
            leftNode->children.erase(it);
        }
    }
    sort(leftNode->children.begin(), leftNode->children.end(), compareNodes);

    //If a parent does not exist, make an empty one
    if(leftNode == root){
        Node* newParent = new Node(nullptr, false, this);
        allNodes.push_back(newParent);
        leftNode->parent = newParent;
        rightNode->parent = newParent;
        newParent->children.push_back(leftNode);
        root = newParent;
    }

    //Update leftNode and rightNode's isLeaf booleans
    if(leftNode->children.size() == 0){
        leftNode->isLeaf = true;
        rightNode->isLeaf = true;
        if(leftNode->nextLeaf != nullptr){
            rightNode->nextLeaf = leftNode->nextLeaf;
        }
        leftNode->nextLeaf = rightNode;
    }

    //If there is a parent, but it is full, recursively call splitNode on the parent
    if(leftNode->parent->isFull()){
        splitNode(leftNode->parent, rightNode->keyValues.begin()->first, rightNode->keyValues.begin()->second);
        sort(leftNode->parent->children.begin(), leftNode->parent->children.end(), compareNodes);
    }

    //Insert rightNode's first key into the parent
    else{
        leftNode->parent->keyValues.insert(pair<int, string>(rightNode->keyValues.begin()->first, rightNode->keyValues.begin()->second));
    }
    //Override pointers adjacent to the newly inserted key to point at the leftNode and the rightNode
    leftNode->parent->children.push_back(rightNode);
    sort(leftNode->parent->children.begin(), leftNode->parent->children.end(), compareNodes);

    //If the rightNode is not a leaf, and it has more than one key, remove rightNode's first value
    if(!(rightNode->isLeaf) && (rightNode->keyValues.size() > 1)){
        rightNode->keyValues.erase(rightNode->keyValues.begin()->first);
    }

    //Update all of the parent pointers in the tree
    updateParentPointers(root);

}

BPlusTree::Node* BPlusTree::findNode(int key){
    Node* currentNode = root;

    while(!currentNode->isLeaf){
        //If the key is less than the first key in the current node
        if(key < currentNode->keyValues.begin()->first){
            //See the first child
            currentNode = *currentNode->children.begin();
            continue;
        }

        //If the key is greater than or equal to the largest key in the current node
        else if(key >= currentNode->keyValues.rbegin()->first){
            //See the last child
            currentNode = *currentNode->children.rbegin();
            continue;
        }

        //Otherwise, find the corresponding child with the correct key
        if(currentNode->keyValues.size()<2){
            return nullptr;
        }

        //Iterate over our keyValues map 
        int currKey;
        int i=0;
        for(map<int, string>::iterator it=currentNode->keyValues.begin(); it != currentNode->keyValues.end(); ++it){
            currKey = it->first;
            //If we find our key to be less than the current key, go to the ith child
            if(key<currKey){
                currentNode = currentNode->children[i];
                break;
            }
            //If we find our key to be the same as the current key, go to the i+1th child
            else if(key==currKey){
                currentNode = currentNode->children[i+1];
                break;
            }
            ++i;
        }
    }
    return currentNode;
}

string BPlusTree::find(int key){
    //Find the node that the key is in
    Node* node = findNode(key);

    //If the key was not found, return null
    if(node == nullptr){
        return "";
    }
    //Otherwise, return the value corresponding with the key in the node
    return node->keyValues.find(key)->second;
}

bool BPlusTree::insert(int key, string value){
    //If the root does not exist, enter the value into it
    if(root == nullptr){   
        root = new Node(nullptr, true, this);
        allNodes.push_back(root);
        root->keyValues.insert(pair<int, string>(key, value));
    }

    //Check if the key is already present
    if(find(key).size() != 0){
        return false;
    }

    //Otherwise, find where to insert our pair
    else{
        Node* insertionNode = findNode(key);
        if(insertionNode->isFull() || insertionNode->isOverflow()){
            splitNode(insertionNode, key, value);
        }
        else{
            insertionNode->keyValues.insert(pair<int, string>(key, value));
        }
    }
    return true;
}  

int BPlusTree::findIndexOfNodeInParent(Node* child){
    Node* parent = child->parent;
    int i=0;
    for(auto it = parent->children.begin(); it != parent->children.end(); it++){
        if(*it == child){
            return i;
        }
        i++;
    }
}

int BPlusTree::findAssociatedKeyOfNodeInParent(Node* child){
    Node* parent = child->parent;
    int index = findIndexOfNodeInParent(child);
    int i=0;
    for(auto it = parent->keyValues.begin(); it!=parent->keyValues.end(); it++){
        if(i == index){
            return(it->first);
        }
        i++;
    }
}

void BPlusTree::redistribute(Node* victim, Node* receiver, bool victimLeftOfReceiver){
    vector<int> keysToRemove;
    vector<Node*> childrenToRemove;
    if(victimLeftOfReceiver){
        keysToRemove.push_back(victim->keyValues.rbegin()->first);
        receiver->keyValues.insert(pair<int, string>(victim->keyValues.rbegin()->first, victim->keyValues.rbegin()->second));

        //Replace the parent value corresponding to the reciever with the new first value in the receiver
        int keyAssociatedWithReceiverInParent = findAssociatedKeyOfNodeInParent(receiver);  //Find the key associated with the receiver in the parent
        Node* parent = receiver->parent;
        parent->keyValues.erase(keyAssociatedWithReceiverInParent);  //Remove the key associated with the receiver in the parent
        parent->keyValues.insert(pair<int, string>(receiver->keyValues.begin()->first, receiver->keyValues.begin()->second));       //Add the fisrt key in the receiver to the parent
        
        if(!receiver->isLeaf){
            childrenToRemove.push_back(*victim->children.rbegin());
            receiver->children.push_back(*victim->children.rbegin());
            sort(receiver->children.begin(), receiver->children.end(), compareNodes);
            sort(victim->children.begin(), victim->children.end(), compareNodes);
        }
    }
    else{
        keysToRemove.push_back(victim->keyValues.begin()->first);
        receiver->keyValues.insert(pair<int, string>(victim->keyValues.begin()->first, victim->keyValues.begin()->second));

        //Replace the parent value corresponding to the victim with the new value in the parent
        int keyAssociatedWithVictimInParent = findAssociatedKeyOfNodeInParent(victim);  //Find the key associated with the victim in the parent
        Node* parent = victim->parent;
        parent->keyValues.erase(keyAssociatedWithVictimInParent);  //Remove the key associated with the victim in the parent
        parent->keyValues.insert(pair<int, string>(victim->keyValues.begin()->first, victim->keyValues.begin()->second));       //Add the fisrt key in the victim to the parent

        if(!receiver->isLeaf){
            childrenToRemove.push_back(*victim->children.begin());
            receiver->children.push_back(*victim->children.begin());
            sort(receiver->children.begin(), receiver->children.end(), compareNodes);
            sort(victim->children.begin(), victim->children.end(), compareNodes);
        }
    }
    for(int i=0; i<keysToRemove.size(); i++){
        victim->keyValues.erase(keysToRemove[i]);
    }
    for(int i=0; i<childrenToRemove.size(); i++){
        auto it = std::find(victim->children.begin(), victim->children.end(), childrenToRemove[i]);
        victim->children.erase(it);
    }
}

BPlusTree::Node* BPlusTree::findLeafToLeftOfNode(Node* node){
    Node* currentNode = root;
    while(!currentNode->isLeaf){
        currentNode = *currentNode->children.begin();
    }
    while(currentNode->nextLeaf != nullptr){
        if(currentNode->nextLeaf == node){
            return currentNode;
        }
        currentNode = currentNode->nextLeaf;
    }
    return nullptr;
}

void BPlusTree::coalesce(Node* victim, Node* receiver, bool victimLeftOfReceiver){
    if(victim->isLeaf){
        for(auto it = victim->keyValues.begin(); it != victim->keyValues.end(); it++){
            receiver->keyValues.insert(pair<int, string>(it->first, it->second));
        }
        if(victimLeftOfReceiver){
            //If there is a leaf to the left of the victim
            Node* leftLeaf = findLeafToLeftOfNode(victim);
            if(leftLeaf != nullptr){
                leftLeaf->nextLeaf = receiver;
            }
        }
        else{
            receiver->nextLeaf = victim->nextLeaf;
        }
    }
    else{
        int counter = 0;
        for(auto it = victim->keyValues.begin(); it != victim->keyValues.end(); it++){
            receiver->keyValues.insert(pair<int, string>(it->first, it->second));
            receiver->children.push_back(victim->children[counter]);
            counter++;
        }
        sort(receiver->children.begin(), receiver->children.end(), compareNodes);
    }  
    int keyAssociatedWithVictim = findAssociatedKeyOfNodeInParent(victim);
    removeFromNode(receiver->parent, keyAssociatedWithVictim, victim);

    //Delete the victim
    auto it = std::find(this->allNodes.begin(), this->allNodes.end(), victim);
    this->allNodes.erase(it);
    delete victim;
}

void BPlusTree::removeFromNode(Node* node, int key, Node* pointer){
    //Erase the pair from the node
    node->keyValues.erase(key);

    //If the node is interior, remove the given pointer
    if(!node->isLeaf){
        vector<Node*>::iterator it = std::find(node->children.begin(), node->children.end(), pointer);
        node->children.erase(it);
        sort(node->children.begin(), node->children.end(), compareNodes);
    }

    //If the node is less than half full
    if(node->keyValues.size() < floor((maxNumPointers-1)/2)){
        //Find left and right siblings
        Node* leftSibling = nullptr;
        Node* rightSibling = nullptr;
        Node* parent = node->parent;
        for(auto it = parent->children.begin(); it != parent->children.end(); it++){
            if((*it) == node){
                if(*prev(it) != nullptr){
                    leftSibling = *prev(it);
                }
                if(*next(it) != nullptr){
                    rightSibling = *next(it);
                }
                break;
            }
        }
        //If the left sibling exists and is more than half full, redistribute from the left sibling
        if(leftSibling != nullptr && leftSibling->keyValues.size() > floor((maxNumPointers-1)/2)){
            redistribute(leftSibling, node, true);
        }
        //Otherwise, if the right sibling exists and is more than half full, redistribute from the right sibling
        else if(rightSibling != nullptr && rightSibling->keyValues.size() > floor((maxNumPointers-1)/2)){
            redistribute(rightSibling, node, false);
        }
        //Otherwise, if the left sibling exists, but is exactly half full, coalesce with the left sibling
        else if(leftSibling != nullptr){
            coalesce(node, leftSibling, false);
        }
        //Otherwise, coalesce with the right sibling
        else{
            coalesce(node, rightSibling, true);
        }
    }
}

bool BPlusTree::remove(int key){
    //Find the associated leaf node
    Node* leaf = findNode(key);

    if(leaf != nullptr){
        removeFromNode(leaf, key, nullptr);
        return true;
    }
    return false;
}

void BPlusTree::printLeaves(){
    Node* currNode = root;
    while(!currNode->isLeaf){
        currNode = *currNode->children.begin();
    }
    while(true){
        cout<<"{";
        int counter = 0;
        for(auto it=currNode->keyValues.begin(); it != currNode->keyValues.end(); it++){
            cout<<"("<<it->first<<", "<<it->second<<")";
            if(counter != currNode->keyValues.size()-1){
                cout<<", ";
            }
            counter++;
        }
        cout<<"}";
        currNode = currNode->nextLeaf;
        if(currNode == nullptr){
            cout<<endl;
            return;
        }
        else{
            cout<<" -> ";
        }
    }
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
            if(!currentNode->children.empty()){
                for(Node* child : currentNode->children){
                    nodesToPrint.push(child);
                }
            }
        }
        cout<<endl<<endl;
    }
    cout<<"--------------------------------\n";
}

void BPlusTree::printNodeKey(Node* node){
    cout<<"|";
    for(map<int, string>::iterator it = node->keyValues.begin(); it != node->keyValues.end(); it++){
        cout<<it->first;
        if(next(it) != node->keyValues.end()){
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
    tree->insert(77, "seventy seven");  //Pointers are breaking here, when I split the parent block, the 45 pointer is given to the new block, rather than staying with the old

    tree->remove(45);   //Not being removed for whatever reason
    tree->printKeys();
}

