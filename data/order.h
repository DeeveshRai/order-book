#include "orderType.h"
#include "side.h"

#ifndef ORDER_H
#define ORDER_H

struct Order {
    int id;
    OrderType type;
    Side side;
    int qty;
    double price;
};

#endif