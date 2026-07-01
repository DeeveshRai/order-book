#ifndef PRICESTORAGE_H
#define PRICESTORAGE_H

#include <map>
#include <vector>
#include "data/orderNode.h"
#include <climits>

class NodeStorage;

class priceStorage{
    public:
        priceStorage(NodeStorage& nodes) : nodes_(nodes) {}

        std::map<int, std::vector<int>> buyPriceLevelStorage;
        std::map<int, std::vector<int>> sellPriceLevelStorage;

        int getSellHeadIndex(int idx);
        int bestAskPrice();
        int bestBidPrice();
        int bestAskHeadIndex();
        int bestBidHeadIndex();
        void updateBuyTail(int price);
        void updateSellTail(int price);
        void updateSellHead(orderNode node);
        void updateBuyHead(orderNode node);
        void updateSellHeadEdgeCase(orderNode node);
        void updateBuyHeadEdgeCase(orderNode node);
        void insertPriceLevel(int price, int prevIndex, int nextIndex, Side side);
        int getBuyTailIndex(int price);
        int getBuyHeadIndex(int price);
        int getSellTailIndex(int price);
        void replaceHead(int idx, int price, Side side);
        void replaceTail(int idx, int price, Side side);
        bool isBuyStoreEmpty();
        bool isBuyPriceLevelEmpty(int price);
        bool isSellStoreEmpty();
        bool isSellPriceLevelEmpty(int price);
        bool sellOneNodeCheck(int price);
        bool buyOneNodeCheck(int price);
        void removePriceLevel(Side side, int price);
        void qtyZeroHandle(orderNode node);
        bool hasMatchableAsks(int price);
        bool hasMatchableBids(int price);

    private:
        NodeStorage& nodes_;
        int bestAsk_ = INT_MAX;
        int bestBid_ = INT_MIN;
        int bestAskHeadIndex_;
        int bestBidHeadIndex_;
        
};

#endif
