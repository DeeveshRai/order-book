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
    orderNode* prev;
    orderNode* next;
};

#endif