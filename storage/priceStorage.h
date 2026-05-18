#include <map>
#include <tuple>
#include "orderNode.h"
using namespace std;

class priceStorage{
    public:
        priceStorage() = default;

        std::map<int, vector<int, int>> buyPriceLevelStorage;
        std::map<int, vector<int, int>> sellPriceLevelStorage;

        int getSellHeadIndex(int idx);
        int bestAskPrice();
        int bestBidPrice();
        void updateBuyTail(int price);
        void updateSellTail(int price);
        int getBuyTailIndex(int price);
        int getSellTailIndex(int price);
        void replaceHead(int idx, int price, Side side);
        void replaceTail(int idx, int price, Side side);
        
};
