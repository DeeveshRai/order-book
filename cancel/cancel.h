#ifndef CANCEL_H
#define CANCEL_H

class NodeStorage;
class priceStorage;
class idMap;

class Cancel {
    public:
        Cancel(NodeStorage& nodes, priceStorage& prices, idMap& ids) 
            : nodes_(nodes), prices_(prices), ids_(ids) {}
        void cancelOrder(int id);

    private:
        NodeStorage& nodes_;
        priceStorage& prices_;
        idMap& ids_;
};

#endif