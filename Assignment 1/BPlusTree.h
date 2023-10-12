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
            public:
                //Public data
                map<int, string> keyValues;     //Here I use a map to store key/value pairs
                vector<Node*> children;       //Will be empty if the Node is a leafNode

                //Constructor
                Node(Node* parent, bool isLeaf, BPlusTree* tree);    
                
                //Helpers
                bool isOverflow() const;        //Checks if the Node is past full
                bool isFull() const;        //Checks if the Node is full
                friend class BPlusTree;
        };
    private:
        Node* root;
        vector<Node*> allNodes;       //Used for memory purposes
        int maxNumPointers; 
    public:
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
        void updateRoot(Node* newRoot);
};