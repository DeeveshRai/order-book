#include "../storage/nodeStorage.h"
#include "../storage/priceStorage.h"
#include "../storage/idMap.h"
#include "../matching/matching.h"
#include "../cancel/cancel.h"

class OrderBook{
    public:
        OrderBook() : prices_(nodes_), ids_(nodes_),
                      matching_(nodes_, prices_, ids_),
                      cancel_(nodes_, prices_, ids_) {}
    
        Matching& getMatching() { return matching_; }
        Cancel& getCancel() { return cancel_; }
        priceStorage& getPrices() { return prices_; }
        NodeStorage& getNodes() { return nodes_; }
        idMap& getIds() { return ids_; }

    private:
        NodeStorage nodes_;
        priceStorage prices_;
        idMap ids_;
        Matching matching_;
        Cancel cancel_;
};