#include <list>
#include <string>
using namespace std;

class Pair{
    private:
        int key;
        string value;
    public:
        Pair(int key, string value);
        string getValue() const;
        int getKey() const;
};