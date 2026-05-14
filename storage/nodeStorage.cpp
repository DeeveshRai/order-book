#include "nodeStorage.h"
#include "priceStorage.h"

std::vector<int> NodeStorage::emptyIndexes(std::vector<orderNode> array){
    //IMPLEMENT LATER
};

priceStorage priceStore;

orderNode NodeStorage::getNode(int price){
    int index = priceStore.getSellHeadIndex(price);
    return store.at(index);
}