#ifndef PRICESTORAGE_H
#define PRICESTORAGE_H

#include <map>
#include <vector>
#include "data/orderNode.h"
#include "data/order.h"

class NodeStorage;

class priceStorage{
    public:
        priceStorage(NodeStorage& nodes) : nodes_(nodes) {}

        std::map<int, std::vector<int>> buyPriceLevelStorage;
        std::map<int, std::vector<int>> sellPriceLevelStorage;

        int getSellHeadIndex(int idx);
        int bestAskPrice();
        int bestBidPrice();
        void updateBuyTail(int price);
        void updateSellTail(int price);
        void addToEmptyBook(int price, int prevIndex, int nextIndex, Side side);
        int getBuyTailIndex(int price);
        int getBuyHeadIndex(int price);
        int getSellTailIndex(int price);
        void replaceHead(int idx, int price, Side side);
        void replaceTail(int idx, int price, Side side);
        bool isBuyStoreEmpty();
        bool isSellStoreEmpty();


    private:
        NodeStorage& nodes_;
        
};

#endif
