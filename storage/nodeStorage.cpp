#include "nodeStorage.h"
#include "priceStorage.h"

std::vector<int> NodeStorage::emptyIndexes(std::vector<orderNode> array){
    //IMPLEMENT LATER
};

priceStorage priceStore;

orderNode NodeStorage::getNode(int index){
    return store[index];
}

int NodeStorage::getSize(){
    return store.size();
}

void NodeStorage::addNode(OrderType type, Side side, int qty, double price, int prev){
    //TODO: IMPLEMENT UUID LOGIC 
    orderNode newNode{1, type, side, qty, price, prev, NULL};
    store.push_back(newNode);
    
    //Sets oldTail's next Index to be NewNode's index
    store[prev].nextIndex = store.size() - 1;
}
