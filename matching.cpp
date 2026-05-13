#include "order.h"
#include "priceStorage.h"
#include "orderNode.h"
#include "nodeStorage.h"

using namespace std;

class Matching {
    public:
        // Attributes
        Matching(NodeStorage& nodes, priceStorage& prices)
            : nodes_(nodes), prices_(prices) {}
        // Methods

    private:
        // Attributes
        NodeStorage& nodes_;
        priceStorage& prices_;

        // Methods
        void checkOrder(Order order);
        void limitBuy(int price, int qty);
        void limitSell();
        void marketBuy();
        void marketSell();
};

void Matching::checkOrder(Order order){
    //Routes based off of order type and buy and sell

    switch (order.type) {
        case LIMIT:
            switch(order.side){
                case BUY: Matching::limitBuy(order.price, order.qty); break;
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

void Matching::limitBuy(int limitOrderPrice, int limitOrderQty){
    //Go to LOWEST price level in BUY PL hashmap (ordered map)
    //Fill orders from lowest price level UNTIL no more valid orders (partial fill - Lack of liquidity) OR order filled
    // IF partial order, store remaining orders within BUY order storage as the latest order within that specific price level
    int remaining = limitOrderQty;

    while (limitOrderQty > 0 && limitOrderPrice >= prices_.bestAskPrice){

        int askPrice = prices_.bestAskPrice();
        int headIdx = prices_.getSellHeadIndex(askPrice);
        
        if (limitOrderQty >= currentNode.qty){
            int temp = limitOrderQty;
            limitOrderQty=-currentNode.qty;
            currentNode.qty-=temp; 
        }
        else {

        }

        orderNode currentNode = nodes_.getNode(counter);
        
    }
    
}

void Matching::limitSell(){
    
}

void Matching::marketBuy(){
    
}

void Matching::marketSell(){
    
}

int main(){

};