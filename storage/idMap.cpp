#include "storage/idMap.h"
#include "storage/nodeStorage.h"
#include "idMap.h"

#include <iostream>

orderNode& idMap::getNodeById(int id){
    // dumpStore();
    int index = idStore[id];
    return nodes_.store[index];

}

void idMap::removeOrderById(int id){
    idStore.erase(id);
}

void idMap::addNodeToStore(int id, int index){
    idStore.insert({id, index});
}

bool idMap::isIdStoreEmpty(){
    return idStore.empty();
}

// void idMap::dumpStore(){
//     for (auto& id : idStore){
//         std::cout << " ids: " << id.first << "\n" << "index" << id.second << "\n";
//     }
// }