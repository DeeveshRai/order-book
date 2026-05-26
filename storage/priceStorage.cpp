#include "storage/priceStorage.h" 
#include "storage/nodeStorage.h"
#include "priceStorage.h"
#include <iostream>

// TODO: Function that replaces head with next

int priceStorage::bestAskPrice(){
    int bestAsk = sellPriceLevelStorage.begin()->first;

    return bestAsk;
}

int priceStorage::bestBidPrice(){
    int bestBid = buyPriceLevelStorage.begin()->first;

    return bestBid;
}

int priceStorage::getBuyHeadIndex(int price){
    int index = buyPriceLevelStorage[price][0];
    return index;
    
}


int priceStorage::getSellHeadIndex(int price){
    int index = sellPriceLevelStorage[price][0];
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
    int newNodeIndex = nodes_.getSize() - 1;
    buyPriceLevelStorage[price][1] = newNodeIndex;
}

void priceStorage::updateSellTail(int price){
    int newNodeIndex = nodes_.getSize()-1;
    sellPriceLevelStorage[price][1] = newNodeIndex;
}

void priceStorage::addToBook(orderNode node){
    if (node.side == Side::BUY){
        //std::map<int, std::vector<int>> buyPriceLevelStorage;
        buyPriceLevelStorage.insert({node.price, {node.prevIndex, node.nextIndex}});
    }

    else{
        sellPriceLevelStorage.insert({node.price, {node.prevIndex, node.nextIndex}});
    }
}

void priceStorage::replaceHead(int idx, int price, Side side){
    if (side == Side::BUY){
        buyPriceLevelStorage[price][0] = idx;
    } 

    else if (side == Side::SELL){
        sellPriceLevelStorage[price][0] = idx;
    }
}

void priceStorage::replaceTail(int idx, int price, Side side){
    if (side == Side::BUY){
        buyPriceLevelStorage[price][1] = idx;
    } 

    else if (side == Side::SELL){
        sellPriceLevelStorage[price][1] = idx;
    }
}

bool priceStorage::isEitherEmpty(){
    return buyPriceLevelStorage.empty() && sellPriceLevelStorage.empty();
}
