#include <list>
#include <string>
#include <map>
using namespace std;

class Node{
    private:
        Node* parent;
        bool isLeaf;
    public:
        map<int, string> keyValues;     //Here I use a map to store key/value pairs
        list<Node*> children;       //Will be empty if the Node is a leafNode
        Node(Node* parent, bool isLeaf);    //Constructor

        friend class BPlusTree;
};

class BPlusTree {
    private:
        Node* root;
        list<Node*> allNodes;       //Used for memory purposes
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
};