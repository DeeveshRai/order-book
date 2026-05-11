#include "order.h"
using namespace std;

class Matching {
    public:
        // Attributes

        // Methods

    private:
        // Attributes

        // Methods
        void checkOrder(Order order);
        void limitBuy();
        void limitSell();
        void marketBuy();
        void marketSell();
};

void Matching::checkOrder(Order order){
    //Routes based off of order type and buy and sell

    switch (order.type) {
        case LIMIT:
            switch(order.side){
                case BUY: Matching::limitBuy(); break;
                case SELL: Matching::limitSell(); break;
            }
            break;

        case MARKET:
            switch(order.side){
                case BUY: Matching::marketBuy(); break;
                case SELL: Matching::marketSell(); break;
            }
            break;
    }

};

void Matching::limitBuy(){
    //Go to LOWEST price level in BUY PL hashmap (unordered map)
    //Fill orders from lowest price level UNTIL no more valid orders (partial fill - Lack of liquidity) OR order filled
    // IF partial order, store remaining orders within BUY order storage as the latest order within that specific price level
    
}

void Matching::limitSell(){
    
}

void Matching::marketBuy(){
    
}

void Matching::marketSell(){
    
}

int main(){
    Order input = {1, LIMIT, BUY, 100, 10.0};
};