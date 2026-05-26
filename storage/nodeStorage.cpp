#include "storage/nodeStorage.h"
#include <cstddef>
#include <iostream>
#include <cstdlib>
#include "nodeStorage.h"

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
    int rand = std::rand() % 10000;

    orderNode newNode{rand, type, side, qty, price, prev, next, true};
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
void NodeStorage::removeNode(int index){
    store[index].active = false;
}