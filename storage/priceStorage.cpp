#include "storage/priceStorage.h" 
#include "storage/nodeStorage.h"
#include "priceStorage.h"
#include <iostream>

int priceStorage::bestAskPrice(){
    if (sellPriceLevelStorage.empty()) return -1;
    return sellPriceLevelStorage.begin()->first;
}

int priceStorage::bestBidPrice(){
    if (buyPriceLevelStorage.empty()) return -1;
    return buyPriceLevelStorage.rbegin()->first;
}

int priceStorage::getBuyHeadIndex(int price){
    auto it = buyPriceLevelStorage.find(price);
    if (it == buyPriceLevelStorage.end()) return -1;
    return it->second[0];
}

int priceStorage::getSellHeadIndex(int price){
    auto it = sellPriceLevelStorage.find(price);
    if (it == sellPriceLevelStorage.end()) return -1;
    return it->second[0];
}

int priceStorage::getBuyTailIndex(int price){
    auto it = buyPriceLevelStorage.find(price);
    if (it == buyPriceLevelStorage.end()) return -1;
    return it->second[1];
}

int priceStorage::getSellTailIndex(int price){
    auto it = sellPriceLevelStorage.find(price);
    if (it == sellPriceLevelStorage.end()) return -1;
    return it->second[1];
}

void priceStorage::updateBuyTail(int price){
    int currTailIndex = getBuyTailIndex(price);
    int newNodeIndex = nodes_.getSize() - 1;
    buyPriceLevelStorage[price][1] = newNodeIndex;
    nodes_.store[currTailIndex].nextIndex = nodes_.store.size() -1;
}


void priceStorage::updateSellTail(int price){
    int currTailIndex = getSellTailIndex(price);
    int newNodeIndex = nodes_.getSize() - 1;
    sellPriceLevelStorage[price][1] = newNodeIndex;
    nodes_.store[currTailIndex].nextIndex = nodes_.store.size() -1;
}

void priceStorage::updateSellHead(orderNode node){
    sellPriceLevelStorage[node.price][0] = node.nextIndex;
}

void priceStorage::updateBuyHead(orderNode node){
    buyPriceLevelStorage[node.price][0] = node.nextIndex;
}

void priceStorage::updateSellHeadEdgeCase(orderNode node){
    sellPriceLevelStorage.erase(node.price);
}

void priceStorage::updateBuyHeadEdgeCase(orderNode node){
    buyPriceLevelStorage.erase(node.price);
}

void priceStorage::addToEmptyBook(int price, int prevIndex, int nextIndex, Side side){
    if (side == Side::BUY){
        buyPriceLevelStorage.insert({price, {prevIndex, nextIndex}});
        
    }

    else if (side == Side::SELL){
        sellPriceLevelStorage.insert({price, {prevIndex, nextIndex}});
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

bool priceStorage::isBuyStoreEmpty(){
    return buyPriceLevelStorage.empty();
}

bool priceStorage::isBuyPriceLevelEmpty(int price){
    return buyPriceLevelStorage.find(price) == buyPriceLevelStorage.end();
}

bool priceStorage::isSellStoreEmpty(){
    return sellPriceLevelStorage.empty();
}

bool priceStorage::isSellPriceLevelEmpty(int price){
    return sellPriceLevelStorage.find(price) == sellPriceLevelStorage.end();
}

bool priceStorage::sellOneNodeCheck(int price){
    return sellPriceLevelStorage[price][0] == sellPriceLevelStorage[price][1];
}

bool priceStorage::buyOneNodeCheck(int price){
    return buyPriceLevelStorage[price][0] == buyPriceLevelStorage[price][1];
}

void priceStorage::removePriceLevel(Side side, int price){
    if (side == Side::BUY){
        buyPriceLevelStorage.erase(price);
    }

    else if (side == Side::SELL){
        sellPriceLevelStorage.erase(price);
    }
}

void priceStorage::qtyZeroHandle(orderNode node){
    if (node.side == Side::BUY && !(buyOneNodeCheck(node.price))){
        if (node.prevIndex == -1){
            updateBuyHead(node);
        }
        else if (node.nextIndex == -1){
            buyPriceLevelStorage[node.price][1] = node.prevIndex;
        }
    }

    if (node.side == Side::SELL && !(sellOneNodeCheck(node.price))){
        if (node.prevIndex == -1){
            updateSellHead(node);
        }
        else if (node.nextIndex == -1){
            sellPriceLevelStorage[node.price][1] = node.prevIndex;
        }
    }

    if (node.side == Side::BUY && buyOneNodeCheck(node.price)){
        removePriceLevel(node.side, node.price);
    }

    else if (node.side == Side::SELL && sellOneNodeCheck(node.price)){
        removePriceLevel(node.side, node.price);
    }

}

bool priceStorage::hasMatchableAsks(int price){

    if (isSellStoreEmpty()) return false;

    return bestAskPrice() <= price;
    
}

bool priceStorage::hasMatchableBids(int price)
{
    if (isBuyStoreEmpty()) return false;

    return bestBidPrice() >= price;
}
