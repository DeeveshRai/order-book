#ifndef PRICESTORAGE_H
#define PRICESTORAGE_H

#include <map>
#include <vector>
#include "data/orderNode.h"

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
        int getBuyTailIndex(int price);
        int getSellTailIndex(int price);
        void replaceHead(int idx, int price, Side side);
        void replaceTail(int idx, int price, Side side);

    private:
        NodeStorage& nodes_;
        
};

#endif
