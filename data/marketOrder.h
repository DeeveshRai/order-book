#include "orderType.h"
#include "side.h"

#ifndef MARKETORDER_H
#define MARKETORDER_H

struct MarketOrder {
    int id;
    OrderType type;
    Side side;
    int qty;
};

#endif