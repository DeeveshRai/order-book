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
        void limitSell(int price, int qty);
        void marketBuy();
        void marketSell();
};

void Matching::checkOrder(Order order){
    //Routes based off of order type and buy and sell

    switch (order.type) {
        case LIMIT:
            switch(order.side){
                case BUY: Matching::limitBuy(order.price, order.qty); break;
                case SELL: Matching::limitSell(order.price, order.qty); break;
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

void Matching::limitBuy(int limitBuyPrice, int limitBuyQty){
    //CRITIQUE: You are still copying nodes accidentally
    int qtyInOrder =  limitBuyQty;
    int bestAsk = prices_.bestAskPrice();
    orderNode bestAskNode = nodes_.getNode(bestAsk);
    int bestAskQty = bestAskNode.qty;

    //Potential empty-book issue
    while (qtyInOrder > 0 && bestAsk && bestAsk <= limitBuyPrice){
        
        if (qtyInOrder <= bestAskNode.qty){
            bestAskQty = bestAskQty - qtyInOrder;
            qtyInOrder = 0;
        } 
        else{
            qtyInOrder = qtyInOrder - bestAskQty;
            bestAskNode = nodes_.getNextNode(bestAskNode.nextIndex);
            bestAsk = bestAskNode.price;
        }
    }

    //Now handle partial
    if (qtyInOrder > 0){
        //ADD NODE TO:
        //1) NodeStorage
        int prev = prices_.getBuyTailIndex(limitBuyPrice); 
        nodes_.addNode(OrderType::LIMIT, Side::BUY, qtyInOrder, limitBuyPrice, prev);
        //2) sellStorage
        prices_.updateBuyTail(limitBuyPrice);
        //To add, must replace BUY tail with new node
        // + newNode.next = None -> Done
        //+ newNode.prev = prevTail - > Done
        // + oldNode.next = newNode -> Done
    }

}

void Matching::limitSell(int limitSellPrice, int limitSellQty){
    
    // int qtyInOrder = limitSellQty;
    // int bestBid = prices_.bestBidPrice();
    // orderNode bestBidNode = nodes_.getNode(bestBid);
    // int bestBidQty = bestBidNode.qty;

    // while (qtyInOrder > 0 && )
}

void Matching::marketBuy(){
    
}

void Matching::marketSell(){
    
}

int main(){

};