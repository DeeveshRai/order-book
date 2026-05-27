#include "matching/matching.h"
#include "storage/nodeStorage.h"
#include "storage/priceStorage.h"
#include <iostream>

using namespace std;

void Matching::checkOrder(Order order){
    if (prices_.isBuyStoreEmpty() && (order.type == OrderType::LIMIT)){
        orderNode node = nodes_.addNode(order.type, order.side, order.qty, order.price, 0, 0);
        
        prices_.addToBook(node);
        return;
    }

    if (prices_.isSellStoreEmpty() && (order.type == OrderType::LIMIT)){
        orderNode node = nodes_.addNode(order.type, order.side, order.qty, order.price, 0, 0);
        
        prices_.addToBook(node);
        return;
    }

    else {

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
    }

};

void Matching::limitBuy(int limitBuyPrice, int limitBuyQty){
    //CRITIQUE: You are still copying nodes accidentally
    int qtyInOrder =  limitBuyQty;
    int bestAsk = prices_.bestAskPrice();
    int bestAskIndex = prices_.getSellHeadIndex(bestAsk);
    orderNode& bestAskNode = nodes_.getNode(bestAskIndex);

    //Potential empty-book issue
    while (bestAskNode.active && qtyInOrder > 0 && bestAsk && bestAsk <= limitBuyPrice){
        
        if (qtyInOrder <= bestAskNode.qty){
            bestAskNode.qty = bestAskNode.qty - qtyInOrder;
            qtyInOrder = 0;
        } 
        else{
            qtyInOrder = qtyInOrder - bestAskNode.qty;
            bestAskNode.active = false;
            bestAskNode.qty = 0;
            bestAskNode = nodes_.getNode(bestAskNode.nextIndex);
            bestAsk = bestAskNode.price;
        }
    }

    //Now handle partial
    if (qtyInOrder > 0){

        if (prices_.buyPriceLevelStorage.empty()){
            orderNode newNode = nodes_.addNode(OrderType::LIMIT, Side::BUY, qtyInOrder, limitBuyPrice, 0, -1);
            prices_.addToBook(newNode);

        }
        else{
            int prev = prices_.getBuyTailIndex(limitBuyPrice); 
            nodes_.addNode(OrderType::LIMIT, Side::BUY, qtyInOrder, limitBuyPrice, prev, -1);
            prices_.updateBuyTail(limitBuyPrice);

        }
    }

}

//CURRENT STATE: Inconsistent linking issues between nodes in price storage and node storage
void Matching::limitSell(int limitSellPrice, int limitSellQty){
    int qtyInOrder = limitSellQty;
    int bestBid = prices_.bestBidPrice();
    int bestBidIndex = prices_.getBuyHeadIndex(bestBid);
    orderNode& bestBidNode = nodes_.getNode(bestBidIndex);

    while (bestBidNode.active && qtyInOrder > 0 && bestBid && bestBid <= limitSellPrice){
        if (qtyInOrder <= bestBidNode.qty){
            bestBidNode.qty = bestBidNode.qty - qtyInOrder;
            qtyInOrder = 0;
        }
        else{
            qtyInOrder = qtyInOrder - bestBidNode.qty; 
            bestBidNode.active = false;
            bestBidNode.qty = 0;
            bestBidNode = nodes_.getNode(bestBidNode.nextIndex);
            bestBid = bestBidNode.price;
        }
    }
    if (qtyInOrder > 0){
        if (prices_.sellPriceLevelStorage.empty()){
            orderNode newNode = nodes_.addNode(OrderType::LIMIT, Side::SELL, qtyInOrder, limitSellPrice, 0, -1);
            prices_.addToBook(newNode);

        }
        else{
            int prev = prices_.getSellTailIndex(limitSellPrice);
            nodes_.addNode(OrderType::LIMIT, Side::SELL, qtyInOrder, limitSellPrice, prev, -1);
            prices_.updateSellTail(limitSellPrice);

        }
    }
}

void Matching::marketBuy(int marketBuyQty){
    
    int qtyInOrder = marketBuyQty;
    int bestBid = prices_.bestBidPrice();
    orderNode& bestBidNode = nodes_.getNode(bestBid);
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
    orderNode& bestBidNode = nodes_.getNode(bestBid);
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

void Matching::doMatching(OrderType type, Side side, int qty, double price){
    Order order = {1, type, side, qty, price};
    checkOrder(order);
    dumpBook();
    
}

void Matching::dumpBook(){
/*
This function will
1) Print all nodes within nodeStorage
2) Print all nodes in order in each of the price storages buy index [head_index] -> ... -> [tail_index]
*/

//NodeStorage

    for (auto& node : nodes_.store){
        std::cout << "node id: " << node.id << "\n" << " node price: " << node.price << "\n" << " node qty: " << node.qty << "\n" << " node prevIndex: " << node.prevIndex <<
        "\n" << " node nextIndex: " << node.nextIndex << "\n" << " node side: " << node.side << "\n" << "node type: "<< node.type << "\n"; 
    }

    for (auto& orderNode : prices_.buyPriceLevelStorage){
        std::cout << " Buy Price: " << orderNode.first << "\n" << " Head: " <<  orderNode.second[0] << "\n" << " Tail: " << orderNode.second[1] << "\n";   
    }

    if (prices_.sellPriceLevelStorage.empty()){
        std::cout << "BAKAKAKAKA" << "\n";
    }
    for (auto& orderNode : prices_.sellPriceLevelStorage){
        std::cout << " Sell Price: " << orderNode.first << "\n" << " Head: " <<  orderNode.second[0] << "\n" << " Tail: " << orderNode.second[1] << "\n";    
    }
}