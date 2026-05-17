#include "priceStorage.h" 
#include "nodeStorage.h"

// TODO: Function that replaces head with next

NodeStorage nodeStorage;


int priceStorage::bestAskPrice(){
    int bestAsk = sellPriceLevelStorage.begin()->first;

    return bestAsk;
}

int priceStorage::bestBidPrice(){
    int bestBid = buyPriceLevelStorage.begin()->first;

    return bestBid;
}

int priceStorage::getSellHeadIndex(int price){
    int index = sellPriceLevelStorage[0][0];
    return index;
    
}

int priceStorage::getBuyTailIndex(int price){
    int index = buyPriceLevelStorage[price][1];
    return index;
}

int priceStorage::getSellTailIndex(int price){
    int index = sellPriceLevelStorage[price][1];
    return index;
}

void priceStorage::updateBuyTail(int price){
    int newNodeIndex = nodeStorage.getSize() - 1;
    buyPriceLevelStorage[price][1] = newNodeIndex;
}

void priceStorage::updateSellTail(int price){
    int newNodeIndex = nodeStorage.getSize()-1;
    sellPriceLevelStorage[price][1] = newNodeIndex;
}
