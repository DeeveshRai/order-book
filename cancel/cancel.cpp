#include "cancel/cancel.h"
#include "storage/idMap.h"
#include "data/orderNode.h"
#include "storage/nodeStorage.h"
#include "storage/priceStorage.h"

#include <iostream>

using namespace std;

void Cancel::cancelOrder(int id){
    orderNode& node = ids_.getNodeById(id);

    //SINGLE NODE PATH
    if (node.prevIndex == -1 && node.nextIndex == -1){
        prices_.removePriceLevel(node.side, node.price);
        ids_.removeOrderById(id);
        node.active = false;
        node.prevIndex = -2;
        node.nextIndex = -2;

    }

    else if (node.prevIndex == -1){
        //Handle replacing head
        orderNode& newNext = nodes_.getNode(node.nextIndex);
        prices_.replaceHead(node.prevIndex, node.price, node.side);
        ids_.removeOrderById(id);
        node.active = false;
        newNext.prevIndex = -1;
        node.prevIndex = -2;

    }
    
    else if(node.nextIndex == -1){
        //Handle replacing tail
        orderNode& newPrev = nodes_.getNode(node.prevIndex);
        prices_.replaceTail(node.nextIndex, node.price, node.side);
        ids_.removeOrderById(id);
        node.active = false;
        newPrev.nextIndex = -1;
        node.nextIndex = -2;

    }
    // MIDDLE HANDLE
    else if (!(node.prevIndex == -1 && node.nextIndex == -1)){
        orderNode& newNext = nodes_.getNode(node.nextIndex);
        orderNode& newPrev = nodes_.getNode(node.prevIndex);

        ids_.removeOrderById(id);
        node.active = false;

        newNext.prevIndex = node.prevIndex;
        newPrev.nextIndex = node.nextIndex;

        node.prevIndex = -2;
        node.nextIndex = -2;

    }
}
