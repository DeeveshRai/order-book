#ifndef NODESTORAGE_H
#define NODESTORAGE_H

#include "data/orderNode.h"
#include <vector>
#include <cstdint>

class NodeStorage {
    public:
        NodeStorage() = default;

        std::vector<orderNode> store;
        orderNode& getNode(int index);
        orderNode addNode(OrderType type, Side side, int qty, double price, int prev, int next);
        int getSize();
        void removeNode(int index);
        void unlinkNode(orderNode node);

    private:
        std::vector<int> emptyIndexes(std::vector<orderNode> array);
        std::uint64_t nextId_ = 1;
        //Make function that gets updates node for next index
        
};

#endif