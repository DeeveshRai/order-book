#include "tests.h"
#include "../storage/nodeStorage.h"
#include "../storage/priceStorage.h"
#include "../storage/idMap.h"
#include "../matching/matching.h"

#include "../data/limitOrder.h"

#include <assert.h>
#include <iostream>

void testExactMatch()
{
    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);

    Matching matching(nodes, prices, ids);

    matching.checkLimitOrder(
        LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});

    matching.checkLimitOrder(
       LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, 10});

    assert(prices.isBuyStoreEmpty());
    assert(prices.isSellStoreEmpty());

    std::cout << "[PASS] ExactMatch\n";
}
int main(){

    testExactMatch();

    std::cout<<"All Tests Pass" << "\n";
}