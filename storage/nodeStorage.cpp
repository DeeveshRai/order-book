#include "storage/nodeStorage.h"
#include <cstddef>
#include <iostream>
#include <cstdlib>
#include "nodeStorage.h"

//TODO: Need to implement actual UUID logic

// std::vector<int> NodeStorage::emptyIndexes(std::vector<orderNode> array){
//     //IMPLEMENT LATER
//     return {};
// };

//DOESNT WORK THE WAY I WANT IT TO -> DO NOT USE
orderNode& NodeStorage::getNode(int index){
    return store[index];
}

int NodeStorage::getSize(){
    return store.size();
}

const orderNode& NodeStorage::addNode(OrderType type, Side side, int qty, double price, int prev, int next){
    //TODO: IMPLEMENT UUID LOGIC 
    int rand = std::rand() % 10000;

    store.push_back({rand, type, side, qty, price, prev, next, true});

    return store.back();
}

void NodeStorage::removeNode(int index){
    store[index].active = false;
}

void NodeStorage::unlinkNode(orderNode& node){

    store[node.prevIndex].nextIndex = node.nextIndex;
    store[node.nextIndex].prevIndex = node.prevIndex;

    node.nextIndex = -2;
    node.prevIndex = -2;
}