#include <map>
#include <tuple>
using namespace std;

class priceStorage{
    public:
        priceStorage() = default;

        std::map<int, tuple<int, int>> buyPriceLevelStorage;
        std::map<int, tuple<int, int>> sellPriceLevelStorage;

        int getSellHeadIndex(int idx);
        int bestAskPrice();
        
};
