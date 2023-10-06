#include <list>
#include <string>
using namespace std;

class Pair{
    private:
        int key;
        string value;
    public:
        //Constructor
        Pair(int key, string value);

        //Getter methods
        string getValue() const;
        int getKey() const;
};

class leafNode{
    private:
        list<Pair> pairs;
        void* parent;
        leafNode* leftSibling;
        leafNode* rightSibling;
    public:
        //Constructor
        leafNode(void* parent, leafNode* leftSibling, leafNode* rightSibling);

        //Getter methods
        void* getParent() const;
        leafNode* getLeftSibling() const;
        leafNode* getRightSibling() const;

        //Setter methods
        void* changeParent(void* newParent);    //Returns a pointer to the old parent
        leafNode* changeLeftSibling(leafNode* newLeftSibling);      //Returns a pointer to the old left sibling
        leafNode* changeRightSibling(leafNode* newRightSibling);    //Returns a pointer to the old right sibling
};