#include <vector>
#include <string>
#include <map>
using namespace std;

class BPlusTree {
    public:
        class Node{
            private:
                Node* parent;
                bool isLeaf;
                BPlusTree* tree;
                Node* nextLeaf;     //Is null if the node is not a leaf itself
            public:
                //Public data
                void* firstChild;       //Only applicable if the node is not a leaf
                map<int, void*> keyPointers;     //Here I use a map to store key/pointer pairs, pointers will be values if node is a leaf

                //Constructor
                Node(Node* parent, bool isLeaf, BPlusTree* tree);    
                
                //Helpers
                bool isOverflow() const;        //Checks if the Node is past full
                bool isFull() const;        //Checks if the Node is full
                friend class BPlusTree;

                //Destructor
                ~Node();
        };
    private:

    public:
        Node* root;
        vector<Node*> allNodes;       //Used for memory purposes
        int maxNumPointers; 

        //Constructor
        BPlusTree(int maxNumPointers);  //Maximum number of entries is maxNumPointers-1

        //Setters     
        bool insert(int key, string value);
        bool remove(int key);

        //Getters
        string find(int key);
        void printKeys();
        void printValues();

        //Helpers
        void printNodeKey(Node* node);  //Prints the keys of the specified node
        void splitNode(Node* leftNode, int key, string value);   //Splits a node to right and left nodes, and inserts a new value
        Node* findNode(int key);        //Finds the leaf node that should contain a given key
        void redistribute(Node* victim, Node* receiver, bool victimLeftOfReceiver);    //Redistributes a value (and a pointer if interior) from the victim to the reciever
        void coalesce(Node* victim, Node* receiver, bool victimLeftOfReceiver);        //Coalesces victim into receiver
        void removeFromNode(Node* node, int key);        //Also removes a pointer if the node is not a leaf
        int findIndexOfNodeInParent(Node* child);
        int findAssociatedKeyOfNodeInParent(Node* child);
        Node* findLeafToLeftOfNode(Node* node);

        //Destructor
        ~BPlusTree();
};