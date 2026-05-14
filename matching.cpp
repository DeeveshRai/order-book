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

    int qtyInOrder =  limitOrderQty;
    int bestAsk = prices_.bestAskPrice();
    orderNode bestAskNode = nodes_.getNode(bestAsk);
    int bestAskQty = bestAskNode.qty;

    while (true){
        

        if (limitOrderQty <= bestAskNode.qty){

            bestAskQty = bestAskQty - limitOrderQty;

        } 
        else{

            qtyInOrder = limitOrderPrice - bestAskQty;
            //Bestask now becomes BestAskNode.nextIndex's price

            //BestAskNode = BestAskNode.nextInd

        }
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