#ifndef TESTS_H
#define TESTS_H

class Test{
    public:

        void testExactMatch();
        void testPartialFillBuy();
        void testPartialFillSell();
        void testMultiLevelMarketBuy();
        void testMultiLevelMarketSell();
        void testFifoPrioSell();
        void testFifoPrioBuy();
        void testHeadRemoval();
        void testCrossingMultiLevels();
        void testNonCrossingOrder();
        void testMarketBuyEmptyBook();
        void testMarketSellEmptyBook();
        void testSingleNodeCancellation();
        void testHeadCancellation();
        void testTailCancellation();
        void testMiddleCancellation();
        void testPriceLevelRemoval();
        void testMixedChaos();


    private:

};

#endif