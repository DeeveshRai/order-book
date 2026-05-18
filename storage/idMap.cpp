#include "idMap.h"
#include "orderNode.h"
#include "nodeStorage.h"
using namespace std;

NodeStorage nodes;

orderNode idMap::getNodeById(int id){
    int index = idStore[id];
    return nodes.getNode(index);

}

void idMap::removeOrderById(int id){
    idStore.erase(id);
}
