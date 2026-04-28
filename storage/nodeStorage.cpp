#include "orderNode.h"
#include <vector>



class NodeStorage {
    public:
        NodeStorage(){
            std::vector<orderNode> store;
        }
    private:
        std::vector<int> emptyIndexes(std::vector<orderNode> array);
    
};

std::vector<int> NodeStorage::emptyIndexes(std::vector<orderNode> array){
    //IMPLEMENT LATER
};