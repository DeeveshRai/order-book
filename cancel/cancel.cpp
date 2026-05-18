#include "cancel/cancel.h"
#include "storage/idMap.h"
#include "data/orderNode.h"
#include "storage/nodeStorage.h"
#include "storage/priceStorage.h"

using namespace std;

NodeStorage nodes;
idMap ids(nodes);

void Cancel::cancelOrder(int id){
    orderNode node = ids.getNodeById(id);
    //This gets the node, now we have to remove it from all stores -> Price/Sell, IDMAP and NodeStorage

    //Price/Sell
    /*
    Check if node.prev or node.next == None
    If it is then it is a head/tail within the storage structures and therefore a new head/tail needs to be put in place
    Else, nothing is needed for price/sell
     */

    //COULD ERROR IF NULL -> FIX
    orderNode newNext = nodes_.getNode(node.nextIndex);
    orderNode newPrev = nodes_.getNode(node.prevIndex);

    if (node.prevIndex == NULL){
        //Handle replacing head
        prices_.replaceHead(node.nextIndex, node.price, node.side);
        newNext.prevIndex = NULL;

    }
    else{
        newNext.prevIndex = node.prevIndex;
    }
    
    if(node.nextIndex == NULL){
        //Handle replacing tail
        prices_.replaceTail(node.nextIndex, node.price, node.side);
        newPrev.nextIndex = NULL;

    }
    else{
        newPrev.nextIndex = node.nextIndex;
    }

    //IdMap removal
    ids.removeOrderById(id);

}
