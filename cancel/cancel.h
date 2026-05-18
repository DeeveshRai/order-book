#ifndef CANCEL_H
#define CANCEL_H

class NodeStorage;
class priceStorage;

class Cancel {
    public:
        Cancel(NodeStorage& nodes, priceStorage& prices) 
            : nodes_(nodes), prices_(prices) {}
        void cancelOrder(int id);

    private:
        NodeStorage& nodes_;
        priceStorage& prices_;
};

#endif