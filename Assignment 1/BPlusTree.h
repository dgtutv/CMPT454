#include <list>
#include <string>
using namespace std;

class Node {
    private:
        string value;
        int key;
        Node* parent;
        list<Node*> children;
        int maxNumPointers;
    public:
        bool isLeaf;
        bool isInterior;
        bool isRoot;

        //Constructor
        Node(int key, string value, int maxNumPointers);

        //Getters
        string getValue() const;
        int getKey() const;
        list<Node*> getChildren() const;
        Node* getParent() const;

        //Setters
        Node* setParent(Node* newParent);   //Returns a pointer to the old parent
        bool addChild(Node* newChild);      //return true on success, false on failure
        Node* removeChild(int value);       //Returns a pointer to the removed child
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