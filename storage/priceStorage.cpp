#include "priceStorage.h" 
#include "nodeStorage.h"

// NodeStorage nodes;
// // This doesnt actually work, because indexes are stored not actual values, must now go to the nodeStorage and find the node stored at that specific node.
// int priceStorage::getBestNode(int index){
//     //Getting the 1st element (the tuple) and then the head (Earliest sell order in at that price level)
//     orderNode order = nodes.getNode(index);
//     return order.price;
// }

// TODO: Function that returns the index

int priceStorage::bestAskPrice(){
    int bestAsk = sellPriceLevelStorage.begin()->first;

    return bestAsk;
}

int priceStorage::getSellHeadIndex(int price){
    int index = 
    return index;

}
