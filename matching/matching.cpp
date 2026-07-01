#include "matching/matching.h"
#include "storage/nodeStorage.h"
#include "storage/priceStorage.h"
#include "storage/idMap.h"
#include <iostream>
#include "matching.h"

// TODO:
// Replace all node/price-level traversal logic
// with getNextMatchableBid()/Ask() helper after V1.
// Basically move all current logic into helper functions

using namespace std;

void Matching::checkLimitOrder(LimitOrder order){
    hasMatchableAskCalls++;
    if ((order.side == Side::BUY) && prices_.isBuyPriceLevelEmpty(order.price) && !prices_.hasMatchableAsks(order.price) && (order.type == OrderType::LIMIT)){  
        orderNode newNode = nodes_.addNode(order.type, order.side, order.qty, order.price, -1, -1);
        ids_.addNodeToStore(newNode.id, nodes_.getSize()-1);
        prices_.insertPriceLevel(order.price, nodes_.getSize() -1, nodes_.getSize() -1, order.side); //Can set PL
        return;
    }

    else if ((order.side == Side::SELL) && !prices_.hasMatchableBids(order.price) && prices_.isSellPriceLevelEmpty(order.price) && (order.type == OrderType::LIMIT)){
        orderNode newNode = nodes_.addNode(order.type, order.side, order.qty, order.price, -1, -1);
        ids_.addNodeToStore(newNode.id, nodes_.getSize()-1);
        prices_.insertPriceLevel(order.price, nodes_.getSize() -1, nodes_.getSize() -1, order.side); //Can set PL
        return;
    }

    else {

        if (order.side == Side::BUY){
            Matching::limitBuy(order.price, order.qty);
        
        }
        
        else if (order.side == Side::SELL){
            Matching::limitSell(order.price, order.qty);
        }
    }

};

void Matching::checkMarketOrder(MarketOrder order){
    if (order.side == Side::BUY){
        if (prices_.isSellStoreEmpty()){
            return;
        }
        else{
            Matching::marketBuy(order.qty);
        }
    }

    else if (order.side == Side::SELL){
        if (prices_.isBuyStoreEmpty()){
            return;
        }

        else{
            Matching::marketSell(order.qty);
        }    
    }
}

void Matching::removeFilledNode(orderNode &node){
    node.qty = 0;
    node.active = false;
    prices_.qtyZeroHandle(node);
    ids_.removeOrderById(node.id);
}


//PLAN: Instead of branching, do function -> FindNextAsk()
//Replace 
void Matching::limitBuy(int limitBuyPrice, int limitBuyQty){

    int qtyInOrder =  limitBuyQty;
    int bestAsk = prices_.bestAskPrice();  
    int bestAskIndex = prices_.getSellHeadIndex(bestAsk);
    int currentIndex = bestAskIndex;

    while (currentIndex != -1 && qtyInOrder > 0 && bestAsk && bestAsk <= limitBuyPrice){

        orderNode& bestAskNode = nodes_.getNode(currentIndex);
        
        if (qtyInOrder < bestAskNode.qty){
            bestAskNode.qty = bestAskNode.qty - qtyInOrder;
            qtyInOrder = 0;
        
        }
        else if(qtyInOrder == bestAskNode.qty){
            //Can mutate sell store - FIXED
            qtyInOrder = 0;
            removeFilledNode(bestAskNode);
        } 
        else{
            qtyInOrder = qtyInOrder - bestAskNode.qty;
            //Very Similar to removeFilledNode helper logic
            bestAskNode.active = false;
            bestAskNode.qty = 0;
            ids_.removeOrderById(bestAskNode.id);
            

            //Can merge the price helper methods so Buy and Sell in same,
            //Then can put all this logic into a matching help
            if (prices_.sellOneNodeCheck(bestAskNode.price)){
                
                prices_.removePriceLevel(Side::SELL, bestAskNode.price);
                
                hasMatchableAskCalls++;
                if (prices_.hasMatchableAsks(limitBuyPrice)){
                    //FindNextAsk
                    bestAsk = prices_.bestAskPrice();
                    bestAskIndex = prices_.getSellHeadIndex(bestAsk); //Need counter
                    currentIndex = bestAskIndex;
                    //FindNextAsk
                    continue;
                }
                
                else break;
            } 
            
            else{
           
                int nextIndex = bestAskNode.nextIndex;
                prices_.updateSellHead(bestAskNode); 
                nodes_.unlinkNode(bestAskNode);
                currentIndex = nextIndex;
                bestAsk = bestAskNode.price;
            }
        }
        
    }

    //Can Replace with addRemainingToBook.
    if (qtyInOrder > 0){

        if (prices_.isBuyPriceLevelEmpty(limitBuyPrice)){
            orderNode newNode = nodes_.addNode(OrderType::LIMIT, Side::BUY, qtyInOrder, limitBuyPrice, -1, -1);
            prices_.insertPriceLevel(limitBuyPrice, nodes_.getSize() -1, nodes_.getSize() -1, Side::BUY);
            int index = prices_.getBuyTailIndex(limitBuyPrice);
            ids_.addNodeToStore(newNode.id, index);

        }
        else{
            int prev = prices_.getBuyTailIndex(limitBuyPrice); 
            orderNode newNode = nodes_.addNode(OrderType::LIMIT, Side::BUY, qtyInOrder, limitBuyPrice, prev, -1);
            prices_.updateBuyTail(limitBuyPrice);
            int index = prices_.getBuyTailIndex(limitBuyPrice);
            ids_.addNodeToStore(newNode.id, index);

        }
    }

}

void Matching::limitSell(int limitSellPrice, int limitSellQty){

    int qtyInOrder = limitSellQty;
    int bestBid = prices_.bestBidPrice();
    int bestBidIndex = prices_.getBuyHeadIndex(bestBid);
    int currentIndex = bestBidIndex;

    while (currentIndex != -1 && qtyInOrder > 0 && bestBid && bestBid >= limitSellPrice){
        orderNode& bestBidNode = nodes_.getNode(currentIndex);

        if (qtyInOrder < bestBidNode.qty){
            bestBidNode.qty = bestBidNode.qty - qtyInOrder;
            qtyInOrder = 0;
        }
        else if(qtyInOrder == bestBidNode.qty){
            qtyInOrder = 0;
            removeFilledNode(bestBidNode);
        }
        else{
            qtyInOrder = qtyInOrder - bestBidNode.qty; 
            bestBidNode.active = false;
            bestBidNode.qty = 0;
            ids_.removeOrderById(bestBidNode.id);
            

            //IF ONLY ONE NODE IN PL
            if (prices_.buyOneNodeCheck(bestBidNode.price)){
                 prices_.removePriceLevel(Side::BUY, bestBidNode.price);

                if (prices_.hasMatchableBids(limitSellPrice)){

                    bestBid = prices_.bestBidPrice();
                    bestBidIndex = prices_.getBuyHeadIndex(bestBid);
                    currentIndex = bestBidIndex;
                    continue;
                }

                else break;
            }

            else{
                int nextIndex = bestBidNode.nextIndex;

                prices_.updateBuyHead(bestBidNode); 
                nodes_.unlinkNode(bestBidNode);
                currentIndex = nextIndex;
                //Get node's index
            }
        }  
    }
    if (qtyInOrder > 0){

        if (prices_.isSellPriceLevelEmpty(limitSellPrice)){
            orderNode newNode = nodes_.addNode(OrderType::LIMIT, Side::SELL, qtyInOrder, limitSellPrice, -1, -1);
            prices_.insertPriceLevel(newNode.price, nodes_.getSize() -1, nodes_.getSize() -1, newNode.side);
            int index = prices_.getSellTailIndex(limitSellPrice);
            ids_.addNodeToStore(newNode.id, index);

        }
        else{
            int prev = prices_.getSellTailIndex(limitSellPrice);
            orderNode newNode = nodes_.addNode(OrderType::LIMIT, Side::SELL, qtyInOrder, limitSellPrice, prev, -1);
            prices_.updateSellTail(limitSellPrice);
            int index = prices_.getSellTailIndex(limitSellPrice);
            ids_.addNodeToStore(newNode.id, index);

        }
    }
}

void Matching::marketBuy(int marketBuyQty){
    int qtyInOrder = marketBuyQty;
    int bestAsk = prices_.bestAskPrice();
    int bestAskIndex = prices_.getSellHeadIndex(bestAsk);
    int currentIndex = bestAskIndex;

    while (qtyInOrder > 0 && bestAsk){   
        orderNode& bestAskNode = nodes_.getNode(currentIndex);

        if (qtyInOrder < bestAskNode.qty){
            bestAskNode.qty = bestAskNode.qty - qtyInOrder;
            qtyInOrder = 0;
        }

        else if (qtyInOrder == bestAskNode.qty){
            bestAskNode.qty = 0;
            qtyInOrder = 0;
            bestAskNode.active = false;
            prices_.qtyZeroHandle(bestAskNode);
            ids_.removeOrderById(bestAskNode.id);
        }
        else{
            qtyInOrder = qtyInOrder - bestAskNode.qty;
            bestAskNode.active = false;
            bestAskNode.qty = 0;
            ids_.removeOrderById(bestAskNode.id);
            

            if (prices_.sellOneNodeCheck(bestAskNode.price)){
                prices_.updateSellHeadEdgeCase(bestAskNode);

                if (!prices_.isSellStoreEmpty()){
                    bestAsk = prices_.bestAskPrice();
                    bestAskIndex = prices_.getSellHeadIndex(bestAsk);
                    currentIndex = bestAskIndex;
                    continue;
                }
                
                else break;
            }
            
            else{
                int nextIndex = bestAskNode.nextIndex;
                
                prices_.updateSellHead(bestAskNode); 
                nodes_.unlinkNode(bestAskNode);
                currentIndex = nextIndex;
            }
        }
    }
}

void Matching::marketSell(int marketSellQty){

    int qtyInOrder = marketSellQty;
    int bestBid = prices_.bestBidPrice();
    int bestBidIndex = prices_.getBuyHeadIndex(bestBid);
    int currentIndex = bestBidIndex;

    while (qtyInOrder > 0 && bestBid){

        orderNode& bestBidNode = nodes_.getNode(currentIndex);


        if (qtyInOrder < bestBidNode.qty){
            bestBidNode.qty = bestBidNode.qty - qtyInOrder;
            qtyInOrder = 0;
        }
        else if (qtyInOrder == bestBidNode.qty){
            bestBidNode.qty = 0;
            qtyInOrder = 0;
            bestBidNode.active = false;
            prices_.qtyZeroHandle(bestBidNode);
            ids_.removeOrderById(bestBidNode.id);
        }
        else{
            qtyInOrder = qtyInOrder - bestBidNode.qty;
            bestBidNode.active = false;
            bestBidNode.qty = 0;
            ids_.removeOrderById(bestBidNode.id);

            if (prices_.buyOneNodeCheck(bestBidNode.price)){
                prices_.updateBuyHeadEdgeCase(bestBidNode);
                
                if (!prices_.isBuyStoreEmpty()){
                    /*Need to move to next price level and begin matching from there
                    */

                    bestBid = prices_.bestBidPrice();
                    bestBidIndex = prices_.getBuyHeadIndex(bestBid);
                    currentIndex = bestBidIndex;
                    continue;
                }

                else break;
            }

            else{

                int nextIndex = bestBidNode.nextIndex;
                prices_.updateBuyHead(bestBidNode);
                nodes_.unlinkNode(bestBidNode);
                currentIndex = nextIndex;
            }
        }
    }
}

void Matching::doLimitMatching(OrderType type, Side side, int qty, double price){
    LimitOrder limitOrder = {1, type, side, qty, price};
    checkLimitOrder(limitOrder);
    dumpBook();
    
}

void Matching::doMarketMatching(OrderType type, Side side, int qty){
    MarketOrder marketOrder = {1, type, side, qty};
    checkMarketOrder(marketOrder);
    dumpBook();
}

void Matching::dumpBook(){
/*
This function will
1) Print all nodes within nodeStorage
2) Print all nodes in order in each of the price storages buy index [head_index] -> ... -> [tail_index]
*/

    for (auto& node : nodes_.store){
        std::cout << "node id: " << node.id << "\n" << " node price: " << node.price << "\n" << " node qty: " << node.qty << "\n" << " node prevIndex: " << node.prevIndex <<
        "\n" << " node nextIndex: " << node.nextIndex << "\n" << " node side: " << node.side << "\n" << "node type: "<< node.type << "\n" << "node active?" << node.active << "\n"; 
    }

    if (prices_.buyPriceLevelStorage.empty()){
        std::cout << "buy empty" << "\n";
    }

    for (auto& orderNode : prices_.buyPriceLevelStorage){
        std::cout << " Buy Price: " << orderNode.first << "\n" << " Head: " <<  orderNode.second[0] << "\n" << " Tail: " << orderNode.second[1] << "\n";   
    }

    if (prices_.sellPriceLevelStorage.empty()){
        std::cout << "sell empty" << "\n";
    }

    for (auto& orderNode : prices_.sellPriceLevelStorage){
        std::cout << " Sell Price: " << orderNode.first << "\n" << " Head: " <<  orderNode.second[0] << "\n" << " Tail: " << orderNode.second[1] << "\n";    
    }

    for (auto& id : ids_.idStore){
        std::cout << " ids: " << id.first << "\n" << "index" << id.second << "\n";
    }
}