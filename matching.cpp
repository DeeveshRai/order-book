#include "order.h"

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
    
}

int main(){
    Order input = {LIMIT, BUY, 100, 10.0};
};