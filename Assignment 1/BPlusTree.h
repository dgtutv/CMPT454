#include <list>
#include <string>
using namespace std;

class Node {
    private:
        string value;
        int key;
    public:
        //Constructor
        Node(int key, string value);

        //Getters
        string getValue() const;
        int getKey() const;
};

class leafNode : public Node {
    private:
        Node* parent;
        list<leafNode*> siblings;
    public:
        //Constructor
        leafNode(int key, string value, Node* parent);

        //Getters
        Node* getParent() const;
        list<leafNode*> getSiblings() const;

        //Setters
        leafNode* addSibling(leafNode* sibling);
        Node* changeParent(Node* newParent);        //Changes the parent of the node and returns the old parent
        Node* removeSibling(leafNode* sibling);     //Removes a sibling of the node and returns a pointer to the sibling upon succession, returns NULL upon failure
};

class interiorNode : public Node {
    private:
        Node* parent;
        list<interiorNode*> siblings;
        list<Node*> children;
    public:
        //Constructor
        interiorNode(int key, string value, Node* parent);

        //Getters
        Node* getParent() const;
        list<interiorNode*> getSiblings() const;
        list<Node*> getChildren() const;

        //Setters
        interiorNode* addSibling(interiorNode* sibling);
        Node* addChild(Node* child);
        Node* changeParent(Node* newParent);        //Changes the parent of the node and returns the old parent
        Node* removeSibling(interiorNode* sibling);     //Removes a sibling of the node and returns a pointer to the sibling upon succession, returns NULL upon failure
        Node* removeChild(Node* child);     //Removes a child of the node and returns a pointer to the child upon succession, returns NULL upon failure
};

class rootNode : public Node {      //Implement
    private:
        list<Node*> children;
    public:
        //Constructor
        rootNode(int key, string value);

        //Getter
        list<Node*> getChildren() const;

        //Setter
        Node* addChild(Node* child);
        Node* removeChild(Node* child);     //Removes a child of the node and returns a pointer to the child upon succession, returns NULL upon failure 
};

class BPlusTree {
    private:
        list<leafNode*> leafNodes;
        list<interiorNode*> interiorNodes;
        rootNode* root;
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