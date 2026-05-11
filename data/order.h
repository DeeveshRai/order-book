#ifndef ORDER.h
#define ORDER.h

enum OrderType { LIMIT, MARKET };
enum Side { BUY, SELL };

struct Order {
    int id;
    OrderType type;
    Side side;
    int qty;
    double price;
};

#endif