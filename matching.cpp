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
        void marketBuy(int qty);
        void marketSell(int qty);
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
                case BUY: Matching::marketBuy(order.qty); break;
                case SELL: Matching::marketSell(order.qty); break;
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
        int prev = prices_.getBuyTailIndex(limitBuyPrice); 
        nodes_.addNode(OrderType::LIMIT, Side::BUY, qtyInOrder, limitBuyPrice, prev);
        prices_.updateBuyTail(limitBuyPrice);
    }

}

void Matching::limitSell(int limitSellPrice, int limitSellQty){
    
    int qtyInOrder = limitSellQty;
    int bestBid = prices_.bestBidPrice();
    orderNode bestBidNode = nodes_.getNode(bestBid);
    int bestBidQty = bestBidNode.qty;

    while (qtyInOrder > 0 && bestBid && bestBid <= limitSellPrice){

        if (qtyInOrder <= bestBidNode.qty){
            bestBidQty = bestBidQty - qtyInOrder;
            qtyInOrder = 0;
        }
        else{
            qtyInOrder = qtyInOrder - bestBidQty;
            bestBidNode = nodes_.getNextNode(bestBidNode.nextIndex);
            bestBid = bestBidNode.price;
        }
    }

    if (qtyInOrder > 0){
        int prev = prices_.getSellTailIndex(limitSellPrice);
        nodes_.addNode(OrderType::LIMIT, Side::SELL, qtyInOrder, limitSellPrice, prev);
        prices_.updateSellTail(limitSellPrice);
    }
}

void Matching::marketBuy(int marketBuyQty){
    
    int qtyInOrder = marketBuyQty;
    int bestBid = prices_.bestBidPrice();
    orderNode bestBidNode = nodes_.getNode(bestBid);
    int bestBidQty = bestBidNode.qty;

    while (qtyInOrder > 0 && bestBid){

        if (qtyInOrder <= bestBidNode.qty){
            bestBidQty = bestBidQty - qtyInOrder;
            qtyInOrder = 0;
        }
        else{
            qtyInOrder = qtyInOrder - bestBidQty;
        }
    }
}

void Matching::marketSell(int marketSellQty){

    int qtyInOrder = marketSellQty;
    int bestBid = prices_.bestBidPrice();
    orderNode bestBidNode = nodes_.getNode(bestBid);
    int bestBidQty = bestBidNode.qty;

    while (qtyInOrder > 0 && bestBid){

        if (qtyInOrder <= bestBidNode.qty){
            bestBidQty = bestBidQty - qtyInOrder;
            qtyInOrder = 0;
        }
        else{
            qtyInOrder = qtyInOrder - bestBidQty;
        }
    }
}

int main(){

};