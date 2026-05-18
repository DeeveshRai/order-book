#include <unordered_map>
using namespace std;

class idMap{
    public:
        idMap() = default;

        std::unordered_map<int, int> idStore;

        orderNode getNodeById(int id);
        void removeOrderById(int id);


};
