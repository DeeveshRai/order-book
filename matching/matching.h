#ifndef MATCHING_H
#define MATCHING_H

#include "data/order.h"

class NodeStorage;
class priceStorage;
class idMap;

class Matching {
    public:
        // Attributes
        Matching(NodeStorage& nodes, priceStorage& prices, idMap& ids)
            : nodes_(nodes), prices_(prices), ids_(ids) {}
        // Methods
        void doMatching(OrderType type, Side side, int qty, double price);
        void dumpBook();

    private:
        // Attributes
        NodeStorage& nodes_;
        priceStorage& prices_;
        idMap& ids_;

        // Methods
        void checkOrder(Order order);
        void limitBuy(int price, int qty);
        void limitSell(int price, int qty);
        void marketBuy(int qty);
        void marketSell(int qty);
        
};

    #endif