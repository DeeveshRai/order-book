#ifndef ORDERNODE.h
#define ORDERNODE.h

enum OrderType { LIMIT, MARKET };
enum Side { BUY, SELL };

struct orderNode {
    int id;
    OrderType type;
    Side side;
    int qty;
    double price;
    int prevIndex;
    int nextIndex;
};

#endif