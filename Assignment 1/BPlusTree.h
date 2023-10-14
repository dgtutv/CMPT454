#include <string>
using namespace std;

class Node{
    private:
        int* keys;
        Node** children;
        string* values;
        Node* parent;
        int size;
        bool isLeaf;
    public:
        Node(int n, bool isLeaf); 

    friend class BPlusTree;
};

class BPlusTree{
    private:
        int maxNumKeys;
        Node* root;
    public:
        BPlusTree(int n);     //Constructor
        BPlusTree(BPlusTree& treeToCopy);       //Copy constructor
        BPlusTree& operator=(BPlusTree& treeToCopy);       //Overloaded equals operator
        ~BPlusTree();       //Destructor

        //Required methods
        bool insert(int key, string value);
        bool remove(int key);
        string find(int key);
        void printKeys();
        void printValues();

        //Helper methods
        Node* copyNode(Node* nodeToCopy);
        void recursiveDelete(Node* node);
        bool insertInLeaf(Node* leftNode, int key, string value);
        bool insertInterior(Node* leftNode, int key, Node* child);
        bool redistribute(Node* victim, Node* receiver);
        bool coalesce(Node* victim, Node* receiver);
}