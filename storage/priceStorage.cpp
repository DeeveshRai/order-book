#include "priceStorage.h" 


// This doesnt actually work, because indexes are stored not actual values, must now go to the nodeStorage and find the node stored at that specific node.
int priceStorage::getPrice(int index){
    //Getting the 1st element (the tuple) and then the head (Earliest sell order in at that price level)
    int sellPrice = get<0>(sellPriceLevelStorage[index]);
    return sellPrice;
}