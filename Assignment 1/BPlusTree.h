#include <list>
#include <string>
using namespace std;

class Node {
    private:
        string value;
        int key;
        Node* parent;
        list<Node*> children;
    public:
        bool isLeaf;
        bool isInterior;
        bool isRoot;

        //Constructor
        Node(int key, string value);

        //Getters
        string getValue() const;
        int getKey() const;
        list<Node*> getChildren() const;
        Node* getParent() const;

        //Setters, return true on success, false on failure
        bool setParent(Node* newParent);
        bool addChild(Node* newChild);
        bool removeChild(int value);
};

class BPlusTree {
    private:
        Node* root;
        list<Node*> allNodes;       //Used for memory purposes
        int maxNumPointers;
    public:
        //Constructor
        BPlusTree(int maxNumPointers);

        //Setters     
        bool insert(int key, string value);
        bool remove(int key);

        //Getters
        string find(int key);
        void printKeys();
        void printValues();
};