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
        Node* changeParent(Node* newParent);        //Implement
};

class interiorNode : public Node {      //Implement
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
};

class rootNode : public Node {

};

class BPlusTree {
    private:
        list<leafNode> leafNodes;
        list<interiorNode> interiorNodes;
        rootNode root;
    public:
};