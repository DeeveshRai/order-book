#include "storage/idMap.h"
#include "storage/nodeStorage.h"

orderNode idMap::getNodeById(int id){
    int index = idStore[id];
    return nodes_.getNode(index);

}

void idMap::removeOrderById(int id){
    idStore.erase(id);
}
