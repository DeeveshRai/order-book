#include "orderType.h"
#include "side.h"

#ifndef LIMITORDER_H
#define LIMITORDER_H

struct LimitOrder {
    int id;
    OrderType type;
    Side side;
    int qty;
    double price;
};

#endif