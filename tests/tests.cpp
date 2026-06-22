#include "orderbook/orderBook.h"

#include <assert.h>
#include <iostream>

//TODO: Need to refactor so there's a OrderBook struct used to instaniate all classes used instead of doing them individually

void testExactMatch()
{
    OrderBook book;

    book.getMatching().checkLimitOrder(
        LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});

    book.getMatching().checkLimitOrder(
       LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, 10});

    assert(book.getPrices().isBuyStoreEmpty());
    assert(book.getPrices().isSellStoreEmpty());

    std::cout << "[PASS] ExactMatch\n";
}

void testPartialFillBuy(){
    OrderBook book;

    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 50, 10});

    assert(!book.getPrices().isBuyStoreEmpty());
    assert(book.getPrices().isSellStoreEmpty());

    int headIndex = book.getPrices().getBuyHeadIndex(10);

    orderNode& node = book.getNodes().getNode(headIndex);

    assert(node.qty == 50);
    assert(node.price == 10);
    assert(node.active);

    std::cout << "[PASS] PartialFillBuy\n";

}

void testPartialFillSell(){
    OrderBook book;

    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 50, 10});

    assert(!book.getPrices().isSellStoreEmpty());
    assert(book.getPrices().isBuyStoreEmpty());

    int headIndex = book.getPrices().getSellHeadIndex(10);

    orderNode& node = book.getNodes().getNode(headIndex);

    assert(node.qty == 50);
    assert(node.price == 10);
    assert(node.active);

    std::cout << "[PASS] PartialFillSell\n";

}

void testMultiLevelMarketBuy(){
    OrderBook book;

    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, 11});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, 12});
    book.getMatching().checkMarketOrder(MarketOrder{1, OrderType::MARKET, Side::BUY, 250});

    assert(book.getPrices().isBuyStoreEmpty());
    assert(!book.getPrices().isSellStoreEmpty());

    int headIndex = book.getPrices().getSellHeadIndex(12);

    orderNode& node = book.getNodes().getNode(headIndex);

    assert(node.qty == 50);
    assert(node.price == 12);
    assert(node.active);

    std::cout << "[PASS] MultiLevelMarketBuy\n";

}

void testMultiLevelMarketSell(){
    OrderBook book;

    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 12});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    book.getMatching().checkMarketOrder(MarketOrder{1, OrderType::MARKET, Side::SELL, 250});

    assert(!book.getPrices().isBuyStoreEmpty());
    assert(book.getPrices().isSellStoreEmpty());

    int headIndex = book.getPrices().getBuyHeadIndex(10);

    orderNode& node = book.getNodes().getNode(headIndex);

    assert(node.qty == 50);
    assert(node.price == 10);
    assert(node.active);

    std::cout << "[PASS] MultiLevelMarketSell\n";

}

void testSellFIFOPriority(){
    OrderBook book;

    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 90, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 80, 10});
    book.getMatching().checkMarketOrder(MarketOrder{1, OrderType::MARKET, Side::BUY, 150});

    assert(book.getPrices().isBuyStoreEmpty());
    assert(!book.getPrices().isSellStoreEmpty());

    int headIndex = book.getPrices().getSellHeadIndex(10);

    orderNode& node = book.getNodes().getNode(headIndex);

    assert(node.qty == 40);
    assert(node.price == 10);
    assert(node.active);

    orderNode& node2 = book.getNodes().getNode(node.nextIndex);
    
    assert(node2.qty == 80);
    assert(node2.price == 10);
    assert(node2.active);

    std::cout << "[PASS] SellFIFOPriority\n";

}

void testBuyFIFOPriority(){
    OrderBook book;

    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 90, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 80, 10});
    book.getMatching().checkMarketOrder(MarketOrder{1, OrderType::MARKET, Side::SELL, 150});

    assert(!book.getPrices().isBuyStoreEmpty());
    assert(book.getPrices().isSellStoreEmpty());

    int headIndex = book.getPrices().getBuyHeadIndex(10);

    orderNode& node = book.getNodes().getNode(headIndex);

    assert(node.qty == 40);
    assert(node.price == 10);
    assert(node.active);

    orderNode& node2 = book.getNodes().getNode(node.nextIndex);
    
    assert(node2.qty == 80);
    assert(node2.price == 10);
    assert(node2.active);

    std::cout << "[PASS] BuyFIFOPriority\n";

}

void testHeadRemoval(){
    OrderBook book;

    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 90, 10});
    book.getMatching().checkMarketOrder(MarketOrder{1, OrderType::MARKET, Side::SELL, 100});

    assert(!book.getPrices().isBuyStoreEmpty());
    assert(book.getPrices().isSellStoreEmpty());

    int headIndex = book.getPrices().getBuyHeadIndex(10);

    orderNode& node = book.getNodes().getNode(headIndex);

    assert(node.qty == 90);
    assert(node.price == 10);
    assert(node.active);

    std::cout << "[PASS] HeadRemoval\n";

}

void testCrossingMultipleLevels(){
    OrderBook book;

    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 11});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 150, 10});
    

    assert(!book.getPrices().isBuyStoreEmpty());
    assert(book.getPrices().isSellStoreEmpty());

    int headIndex = book.getPrices().getBuyHeadIndex(10);

    orderNode& node = book.getNodes().getNode(headIndex);

    assert(node.qty == 50);
    assert(node.price == 10);
    assert(node.active);

    std::cout << "[PASS] CrossingMultipleLevels\n";
}

void testNonCrossingOrder(){
    OrderBook book;

    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 11});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 150, 11});
    

    assert(!book.getPrices().isBuyStoreEmpty());
    assert(!book.getPrices().isSellStoreEmpty());

    int headIndex = book.getPrices().getBuyHeadIndex(10);

    orderNode& node = book.getNodes().getNode(headIndex);

    assert(node.qty == 100);
    assert(node.price == 10);
    assert(node.active);

    headIndex = book.getPrices().getSellHeadIndex(11);

    orderNode& sellNode = book.getNodes().getNode(headIndex);

    assert(sellNode.qty == 50);
    assert(sellNode.price == 11);
    assert(sellNode.active);

    std::cout << "[PASS] NonCrossingMultipleLevels\n";
}

void testMarketBuyEmptyBook(){
    OrderBook book;

    book.getMatching().checkMarketOrder(MarketOrder{1, OrderType::MARKET, Side::BUY, 100});

    assert(book.getPrices().isBuyStoreEmpty());
    assert(book.getPrices().isSellStoreEmpty());

    std::cout << "[PASS] MarketBuyBookEmpty\n";
}

void testMarketSellEmptyBook(){
    OrderBook book;

    book.getMatching().checkMarketOrder(MarketOrder{1, OrderType::MARKET, Side::SELL, 100});

    assert(book.getPrices().isBuyStoreEmpty());
    assert(book.getPrices().isSellStoreEmpty());

    std::cout << "[PASS] MarketSellBookEmpty\n";
}

void testSingleNodeCancellation(){
    OrderBook book;

    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});

    int headIndex = book.getPrices().getBuyHeadIndex(10);

    orderNode& node = book.getNodes().getNode(headIndex);

    book.getCancel().cancelOrder(node.id);

    assert(book.getPrices().isBuyStoreEmpty());
    assert(book.getPrices().isSellStoreEmpty());
    assert(book.getIds().isIdStoreEmpty());

    std::cout << "[PASS] SingleNodeCancellation\n";

}

void testHeadCancellation(){
    OrderBook book;

    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 90, 10});

    int headIndex = book.getPrices().getBuyHeadIndex(10);

    orderNode& node = book.getNodes().getNode(headIndex);

    book.getCancel().cancelOrder(node.id);

    assert(!book.getPrices().isBuyStoreEmpty());
    assert(book.getPrices().isSellStoreEmpty());

    headIndex = book.getPrices().getBuyHeadIndex(10);

    orderNode& node2 = book.getNodes().getNode(headIndex);

    assert(node2.qty == 90);
    assert(node2.price == 10);
    assert(node2.active);

    std::cout << "[PASS] HeadCancellation\n";

}

void testTailCancellation(){
    OrderBook book;

    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 90, 10});

    int tailIndex = book.getPrices().getBuyTailIndex(10);

    orderNode& node = book.getNodes().getNode(tailIndex);

    book.getCancel().cancelOrder(node.id);

    assert(!book.getPrices().isBuyStoreEmpty());
    assert(book.getPrices().isSellStoreEmpty());

    tailIndex = book.getPrices().getBuyTailIndex(10);

    orderNode& node2 = book.getNodes().getNode(tailIndex);

    assert(node2.qty == 100);
    assert(node2.price == 10);
    assert(node2.active);

    std::cout << "[PASS] TailCancellation\n";
}

void testMiddleCancellation(){
    OrderBook book;

    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 90, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 80, 10});

    int headIndex = book.getPrices().getBuyHeadIndex(10);

    orderNode& node = book.getNodes().getNode(headIndex);

    int midIndex = node.nextIndex;

    orderNode& midNode = book.getNodes().getNode(midIndex);

    book.getCancel().cancelOrder(midNode.id);

    assert(!book.getPrices().isBuyStoreEmpty());
    assert(book.getPrices().isSellStoreEmpty());

    headIndex = book.getPrices().getBuyHeadIndex(10);
    int tailIndex = book.getPrices().getBuyTailIndex(10);

    orderNode& headNode = book.getNodes().getNode(headIndex);
    orderNode& tailNode = book.getNodes().getNode(tailIndex);
    
    assert(headNode.qty == 100);
    assert(headNode.nextIndex == tailIndex);
    assert(tailNode.qty == 80);
    assert(tailNode.prevIndex == headIndex);

    std::cout << "[PASS] MiddleCancellation\n";

}

void testPriceLevelRemoval(){
    OrderBook book;

    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, 10});

    assert(book.getPrices().isBuyPriceLevelEmpty(10));
    assert(book.getPrices().isBuyStoreEmpty());
    assert(book.getPrices().isSellStoreEmpty());

    std::cout << "[PASS] PriceLevelRemoval\n";
}

void testMixedChaos(){
    OrderBook book;

    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 90, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 80, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 50, 10});

    int headIndex = book.getPrices().getBuyHeadIndex(10);

    orderNode& node = book.getNodes().getNode(headIndex);

    int midIndex = node.nextIndex;

    orderNode& midNode = book.getNodes().getNode(midIndex);

    book.getCancel().cancelOrder(midNode.id);

    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 60, 10});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::BUY, 120, 10});
    book.getMatching().checkMarketOrder(MarketOrder{1, OrderType::MARKET, Side::SELL, 170});
    book.getMatching().checkLimitOrder(LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, 11});
    book.getMatching().checkMarketOrder(MarketOrder{1, OrderType::MARKET, Side::BUY, 200});

    assert(!book.getPrices().isBuyStoreEmpty());
    assert(!book.getPrices().isBuyPriceLevelEmpty(10));
    assert(book.getPrices().isSellStoreEmpty());

    headIndex = book.getPrices().getBuyHeadIndex(10);

    orderNode& headNode = book.getNodes().getNode(headIndex);

    assert(headNode.qty == 20);
    assert(headNode.price == 10);
    assert(headNode.active);
    assert(headNode.nextIndex == -1);
    assert(headNode.prevIndex == -1);

    assert(!book.getIds().isIdStoreEmpty());
    assert(book.getIds().idStore[headNode.id] == headIndex);

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

