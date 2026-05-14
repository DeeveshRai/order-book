#include "orderNode.h"
#include <vector>
using namespace std;

class NodeStorage {
    public:
        NodeStorage() = default;
        std::vector<orderNode> store;
        orderNode getNode(int index);

    private:
        std::vector<int> emptyIndexes(std::vector<orderNode> array);
        //Make function that gets updates node for next index
        
};