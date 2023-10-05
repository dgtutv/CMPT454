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
};

class interiorNode : public Node {

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