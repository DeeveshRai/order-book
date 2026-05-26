#include "orderType.h"
#include "side.h"

#ifndef ORDERNODE_H
#define ORDERNODE_H

struct orderNode {
    int id;
    OrderType type;
    Side side;
    int qty;
    double price;
    int prevIndex;
    int nextIndex;
    bool active;
};

#endif