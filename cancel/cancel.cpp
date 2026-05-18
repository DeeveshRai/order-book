#include "cancel.h"
#include "idMap.h"
#include "orderNode.h"
#include "nodeStorage.h"
#include "priceStorage.h"

using namespace std;

idMap ids;
NodeStorage nodes;
priceStorage prices;

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
    orderNode newNext = nodes.getNode(node.nextIndex);
    orderNode newPrev = nodes.getNode(node.prevIndex);

    if (node.prevIndex == NULL){
        //Handle replacing head
        prices.replaceHead(node.nextIndex, node.price, node.side);
        newNext.prevIndex = NULL;

    }
    else{
        newNext.prevIndex = node.prevIndex;
    }
    
    if(node.nextIndex == NULL){
        //Handle replacing tail
        prices.replaceTail(node.nextIndex, node.price, node.side);
        newPrev.nextIndex = NULL;

    }
    else{
        newPrev.nextIndex = node.nextIndex;
    }

    //IdMap removal
    ids.removeOrderById(id);

}
