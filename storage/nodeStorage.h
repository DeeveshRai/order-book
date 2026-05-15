#include "orderNode.h"
#include <vector>
using namespace std;

class NodeStorage {
    public:
        NodeStorage() = default;
        std::vector<orderNode> store;
        orderNode getNode(int index);
        orderNode getNextNode(int index);
        // orderNode createNode(Side side, OrderType type);
        void addNode(OrderType type, Side side, int qty, double price, int prev);
        int getSize();

    private:
        std::vector<int> emptyIndexes(std::vector<orderNode> array);
        //Make function that gets updates node for next index
        
};

// TODO: Need to add functions to handle removal of nodes with < 0 qty