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
        void addSibling(leafNode* sibling);
        Node* changeParent(Node* newParent);   
        Node* removeSibling(leafNode* sibling);     //Implement
        Node* removeChild(Node* child);     //Implement     
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
        void addSibling(interiorNode* sibling);
        void addChild(Node* child);
        Node* changeParent(Node* newParent);
        Node* removeSibling(interiorNode* sibling);     //Implement
        Node* removeChild(Node* child);     //Implement
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
        void addChild(Node* child);
        Node* removeChild(Node* child);
};

class BPlusTree {
    private:
        list<leafNode*> leafNodes;
        list<interiorNode*> interiorNodes;
        rootNode* root;
        list<Node*> allNodes;       //Used for memory purposes
    public:
};