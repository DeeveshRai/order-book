#ifndef IDMAP_H
#define IDMAP_H

#include <unordered_map>
#include "data/orderNode.h"

class NodeStorage;

class idMap{
    public:
        idMap(NodeStorage& nodes) : nodes_(nodes) {}

        std::unordered_map<int, int> idStore;

        orderNode getNodeById(int id);
        void removeOrderById(int id);

    private:
        NodeStorage& nodes_;

};

#endif
