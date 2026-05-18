#ifndef MATCHING_H
#define MATCHING_H

#include "data/order.h"

class NodeStorage;
class priceStorage;

class Matching {
    public:
        // Attributes
        Matching(NodeStorage& nodes, priceStorage& prices)
            : nodes_(nodes), prices_(prices) {}
        // Methods
        void doMatching(OrderType type, Side side, int qty, double price);

    private:
        // Attributes
        NodeStorage& nodes_;
        priceStorage& prices_;

        // Methods
        void checkOrder(Order order);
        void limitBuy(int price, int qty);
        void limitSell(int price, int qty);
        void marketBuy(int qty);
        void marketSell(int qty);
        
};

    #endif