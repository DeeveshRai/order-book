#include "nodeStorage.h"
#include "priceStorage.h"

std::vector<int> NodeStorage::emptyIndexes(std::vector<orderNode> array){
    //IMPLEMENT LATER
};

priceStorage priceStore;

orderNode NodeStorage::getNode(int idx){
    int index = priceStore.getSellIndex(idx);
    return store.at(index);
}