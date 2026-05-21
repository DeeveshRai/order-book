#include "storage/nodeStorage.h"
#include <cstddef>
#include <iostream>

std::vector<int> NodeStorage::emptyIndexes(std::vector<orderNode> array){
    //IMPLEMENT LATER
    return {};
};

orderNode& NodeStorage::getNode(int index){
    return store[index];
}

int NodeStorage::getSize(){
    return store.size();
}

orderNode NodeStorage::addNode(OrderType type, Side side, int qty, double price, int prev, int next){
    //TODO: IMPLEMENT UUID LOGIC 
    orderNode newNode{1, type, side, qty, price, prev, next};
    store.push_back(newNode);
    
    //Sets oldTail's next Index to be NewNode's index
    store[prev].nextIndex = store.size() - 1;

    return newNode;
}

void NodeStorage::printNodes(){
    for (auto& node : store){
        std::cout << node.qty;
    }

}