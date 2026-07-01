#ifndef MATCHING_H
#define MATCHING_H

#include "data/limitOrder.h"
#include "data/marketOrder.h"
#include "data/orderNode.h"
#include <cstdint>

class NodeStorage;
class priceStorage;
class idMap;

class Matching {
    public:
        // Attributes
        Matching(NodeStorage& nodes, priceStorage& prices, idMap& ids)
            : nodes_(nodes), prices_(prices), ids_(ids) {}
        // Methods
        void doLimitMatching(OrderType type, Side side, int qty, double price);
        void doMarketMatching(OrderType type, Side side, int qty);
        void checkLimitOrder(LimitOrder order);
        void checkMarketOrder(MarketOrder order);
        void removeFilledNode(orderNode& node);
        void dumpBook();

        std::uint64_t bestBidCalls_ = 0;
        std::uint64_t bestAskCalls = 0;
        std::uint64_t hasMatchableBidCalls = 0;
        std::uint64_t hasMatchableAskCalls = 0;
        std::uint64_t getHeadCalls = 0;
        std::uint64_t erasePriceLevelCalls = 0;

    private:
        // Attributes
        NodeStorage& nodes_;
        priceStorage& prices_;
        idMap& ids_;

        // Methods
        
        
        void limitBuy(int price, int qty);
        void limitSell(int price, int qty);
        void marketBuy(int qty);
        void marketSell(int qty);
        
};

    #endif