#include <map>
#include <tuple>
using namespace std;

class priceStorage{
    public:
        std::map<int, tuple<int, int>> buyPriceLevelStorage;
        std::map<int, tuple<int, int>> sellPriceLevelStorage;
};