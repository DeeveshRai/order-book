#include "../storage/nodeStorage.h"
#include "../storage/priceStorage.h"
#include "../storage/idMap.h"
#include "../matching/matching.h"
#include "../cancel/cancel.h"

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

void testPartialFillBuy(){
    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);

    Matching matching(nodes, prices, ids);

    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 50, 10});;

    assert(!prices.isBuyStoreEmpty());
    assert(prices.isSellStoreEmpty());

    int headIndex = prices.getBuyHeadIndex(10);

    orderNode& node = nodes.getNode(headIndex);

    assert(node.qty == 50);
    assert(node.price == 10);
    assert(node.active);

    std::cout << "[PASS] PartialFillBuy\n";

}

void testPartialFillSell(){
    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);
    Matching matching(nodes, prices, ids);

    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 50, 10});

    assert(!prices.isSellStoreEmpty());
    assert(prices.isBuyStoreEmpty());

    int headIndex = prices.getSellHeadIndex(10);

    orderNode& node = nodes.getNode(headIndex);

    assert(node.qty == 50);
    assert(node.price == 10);
    assert(node.active);

    std::cout << "[PASS] PartialFillSell\n";

}

void testMultiLevelMarketBuy(){
    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);
    Matching matching(nodes, prices, ids);

    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, 11});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, 12});
    matching.checkMarketOrder(MarketOrder{1, OrderType::MARKET, Side::BUY, 250});

    assert(prices.isBuyStoreEmpty());
    assert(!prices.isSellStoreEmpty());

    int headIndex = prices.getSellHeadIndex(12);

    orderNode& node = nodes.getNode(headIndex);

    assert(node.qty == 50);
    assert(node.price == 12);
    assert(node.active);

    std::cout << "[PASS] MultiLevelMarketBuy\n";

}

void testMultiLevelMarketSell(){
    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);
    Matching matching(nodes, prices, ids);

    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 12});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    matching.checkMarketOrder(MarketOrder{1, OrderType::MARKET, Side::SELL, 250});

    assert(!prices.isBuyStoreEmpty());
    assert(prices.isSellStoreEmpty());

    int headIndex = prices.getBuyHeadIndex(10);

    orderNode& node = nodes.getNode(headIndex);

    assert(node.qty == 50);
    assert(node.price == 10);
    assert(node.active);

    std::cout << "[PASS] MultiLevelMarketSell\n";

}

void testSellFIFOPriority(){
    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);
    Matching matching(nodes, prices, ids);

    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 90, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 80, 10});
    matching.checkMarketOrder(MarketOrder{1, OrderType::MARKET, Side::BUY, 150});

    assert(prices.isBuyStoreEmpty());
    assert(!prices.isSellStoreEmpty());

    int headIndex = prices.getSellHeadIndex(10);

    orderNode& node = nodes.getNode(headIndex);

    assert(node.qty == 40);
    assert(node.price == 10);
    assert(node.active);

    orderNode& node2 = nodes.getNode(node.nextIndex);
    
    assert(node2.qty == 80);
    assert(node2.price == 10);
    assert(node2.active);

    std::cout << "[PASS] SellFIFOPriority\n";

}

void testBuyFIFOPriority(){
    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);
    Matching matching(nodes, prices, ids);

    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 90, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 80, 10});
    matching.checkMarketOrder(MarketOrder{1, OrderType::MARKET, Side::SELL, 150});

    assert(!prices.isBuyStoreEmpty());
    assert(prices.isSellStoreEmpty());

    int headIndex = prices.getBuyHeadIndex(10);

    orderNode& node = nodes.getNode(headIndex);

    assert(node.qty == 40);
    assert(node.price == 10);
    assert(node.active);

    orderNode& node2 = nodes.getNode(node.nextIndex);
    
    assert(node2.qty == 80);
    assert(node2.price == 10);
    assert(node2.active);

    std::cout << "[PASS] BuyFIFOPriority\n";

}

void testHeadRemoval(){
    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);
    Matching matching(nodes, prices, ids);

    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 90, 10});
    matching.checkMarketOrder(MarketOrder{1, OrderType::MARKET, Side::SELL, 100});

    assert(!prices.isBuyStoreEmpty());
    assert(prices.isSellStoreEmpty());

    int headIndex = prices.getBuyHeadIndex(10);

    orderNode& node = nodes.getNode(headIndex);

    assert(node.qty == 90);
    assert(node.price == 10);
    assert(node.active);

    std::cout << "[PASS] HeadRemoval\n";

}

void testCrossingMultipleLevels(){
    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);
    Matching matching(nodes, prices, ids);

    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 11});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 150, 10});
    

    assert(!prices.isBuyStoreEmpty());
    assert(prices.isSellStoreEmpty());

    int headIndex = prices.getBuyHeadIndex(10);

    orderNode& node = nodes.getNode(headIndex);

    assert(node.qty == 50);
    assert(node.price == 10);
    assert(node.active);

    std::cout << "[PASS] CrossingMultipleLevels\n";
}

void testNonCrossingOrder(){
    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);
    Matching matching(nodes, prices, ids);

    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 11});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 150, 11});
    

    assert(!prices.isBuyStoreEmpty());
    assert(!prices.isSellStoreEmpty());

    int headIndex = prices.getBuyHeadIndex(10);

    orderNode& node = nodes.getNode(headIndex);

    assert(node.qty == 100);
    assert(node.price == 10);
    assert(node.active);

    headIndex = prices.getSellHeadIndex(11);

    orderNode& sellNode = nodes.getNode(headIndex);

    assert(sellNode.qty == 50);
    assert(sellNode.price == 11);
    assert(sellNode.active);

    std::cout << "[PASS] NonCrossingMultipleLevels\n";
}

void testMarketBuyEmptyBook(){
    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);
    Matching matching(nodes, prices, ids);

    matching.checkMarketOrder(MarketOrder{1, OrderType::MARKET, Side::BUY, 100});

    assert(prices.isBuyStoreEmpty());
    assert(prices.isSellStoreEmpty());

    std::cout << "[PASS] MarketBuyBookEmpty\n";
}

void testMarketSellEmptyBook(){
    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);
    Matching matching(nodes, prices, ids);

    matching.checkMarketOrder(MarketOrder{1, OrderType::MARKET, Side::SELL, 100});

    assert(prices.isBuyStoreEmpty());
    assert(prices.isSellStoreEmpty());

    std::cout << "[PASS] MarketSellBookEmpty\n";
}

void testSingleNodeCancellation(){
    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);
    Matching matching(nodes, prices, ids);
    Cancel cancel(nodes, prices, ids);

    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});

    int headIndex = prices.getBuyHeadIndex(10);

    orderNode& node = nodes.getNode(headIndex);

    cancel.cancelOrder(node.id);

    assert(prices.isBuyStoreEmpty());
    assert(prices.isSellStoreEmpty());
    assert(ids.isIdStoreEmpty());

    std::cout << "[PASS] SingleNodeCancellation\n";

}

void testHeadCancellation(){
    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);
    Matching matching(nodes, prices, ids);
    Cancel cancel(nodes, prices, ids);

    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 90, 10});

    int headIndex = prices.getBuyHeadIndex(10);

    orderNode& node = nodes.getNode(headIndex);

    cancel.cancelOrder(node.id);

    assert(!prices.isBuyStoreEmpty());
    assert(prices.isSellStoreEmpty());

    headIndex = prices.getBuyHeadIndex(10);

    orderNode& node2 = nodes.getNode(headIndex);

    assert(node2.qty == 90);
    assert(node2.price == 10);
    assert(node2.active);

    std::cout << "[PASS] HeadCancellation\n";

}

void testTailCancellation(){
    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);
    Matching matching(nodes, prices, ids);
    Cancel cancel(nodes, prices, ids);

    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 90, 10});

    int tailIndex = prices.getBuyTailIndex(10);

    orderNode& node = nodes.getNode(tailIndex);

    cancel.cancelOrder(node.id);

    assert(!prices.isBuyStoreEmpty());
    assert(prices.isSellStoreEmpty());

    tailIndex = prices.getBuyTailIndex(10);

    orderNode& node2 = nodes.getNode(tailIndex);

    assert(node2.qty == 100);
    assert(node2.price == 10);
    assert(node2.active);

    std::cout << "[PASS] TailCancellation\n";
}

void testMiddleCancellation(){
    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);
    Matching matching(nodes, prices, ids);
    Cancel cancel(nodes, prices, ids);

    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 90, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 80, 10});

    int headIndex = prices.getBuyHeadIndex(10);

    orderNode& node = nodes.getNode(headIndex);

    int midIndex = node.nextIndex;

    orderNode& midNode = nodes.getNode(midIndex);

    cancel.cancelOrder(midNode.id);

    assert(!prices.isBuyStoreEmpty());
    assert(prices.isSellStoreEmpty());

    headIndex = prices.getBuyHeadIndex(10);
    int tailIndex = prices.getBuyTailIndex(10);

    orderNode& headNode = nodes.getNode(headIndex);
    orderNode& tailNode = nodes.getNode(tailIndex);
    
    assert(headNode.qty == 100);
    assert(headNode.nextIndex == tailIndex);
    assert(tailNode.qty == 80);
    assert(tailNode.prevIndex == headIndex);

    std::cout << "[PASS] MiddleCancellation\n";

}

void testPriceLevelRemoval(){
    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);
    Matching matching(nodes, prices, ids);

    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, 10});

    assert(prices.isBuyPriceLevelEmpty(10));
    assert(prices.isBuyStoreEmpty());
    assert(prices.isSellStoreEmpty());

    std::cout << "[PASS] PriceLevelRemoval\n";
}

void testMixedChaos(){
    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);
    Matching matching(nodes, prices, ids);
    Cancel cancel(nodes, prices, ids);

    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 90, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 80, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 50, 10});

    int headIndex = prices.getBuyHeadIndex(10);

    orderNode& node = nodes.getNode(headIndex);

    int midIndex = node.nextIndex;

    orderNode& midNode = nodes.getNode(midIndex);

    cancel.cancelOrder(midNode.id);

    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 60, 10});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 120, 10});
    matching.checkMarketOrder(MarketOrder{1, OrderType::MARKET, Side::SELL, 170});
    matching.checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, 11});
    matching.checkMarketOrder(MarketOrder{1, OrderType::MARKET, Side::BUY, 200});

    assert(!prices.isBuyStoreEmpty());
    assert(!prices.isBuyPriceLevelEmpty(10));
    assert(prices.isSellStoreEmpty());

    headIndex = prices.getBuyHeadIndex(10);

    orderNode& headNode = nodes.getNode(headIndex);

    assert(headNode.qty == 20);
    assert(headNode.price == 10);
    assert(headNode.active);
    assert(headNode.nextIndex == -1);
    assert(headNode.prevIndex == -1);

    assert(!ids.isIdStoreEmpty());
    assert(ids.idStore[headNode.id] == headIndex);

    std::cout << "[PASS] MixedChaos\n";
}

int main(){

    testExactMatch();
    testPartialFillBuy();
    testPartialFillSell();
    testMultiLevelMarketBuy();
    testMultiLevelMarketSell();
    testSellFIFOPriority();
    testBuyFIFOPriority();
    testHeadRemoval();
    testCrossingMultipleLevels();
    testNonCrossingOrder();
    testMarketBuyEmptyBook();
    testMarketSellEmptyBook();
    testSingleNodeCancellation();
    testHeadCancellation();
    testTailCancellation();
    testMiddleCancellation();
    testPriceLevelRemoval();
    testMixedChaos();

    std::cout<<"All Tests Pass" << "\n";
}

