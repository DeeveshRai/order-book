#include <chrono>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include "orderbook/orderBook.h"

//use steady_clock
//Find time before running 1mill orders
//Find time after running 1mill orders
struct ScopedTimer {
   std::chrono::time_point<std::chrono::steady_clock> start;

   ScopedTimer(){
    start = std::chrono::steady_clock::now();
   }

   ~ScopedTimer(){
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed = end - start;

    std::cout << std::fixed << std::setprecision(9) 
              << "Elapsed Time: " << elapsed.count() << "s\n" << std::flush;
   }

};

int main(){
    std::srand(std::time(0) + std::clock());
    
    OrderBook book;

    //Inserting 1mill orders
    // for (int i = 0; i < 1000000; i++){
    //     //Need to add 1mill orders

    //     book.getMatching().checkLimitOrder(
    //         LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});

    // }

    //Matching 1mill Orders with price = 100
    //     for (int i = 0; i < 1000000; i++){
    //     //Need to add 1mill orders

    //     book.getMatching().checkLimitOrder(
    //         LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 10});
        
    //     book.getMatching().checkLimitOrder(
    //         LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, 10});

    // }

    //Matching 1 mill orders with price = random

    // std::vector<double> buyPrices;
    // std::vector<double> sellPrices;

    // buyPrices.reserve(1000000);
    // sellPrices.reserve(1000000);

    // for (int i = 0; i < 1000000; i++) {
    //     buyPrices.push_back(rand() % 1000);
    //     sellPrices.push_back(rand() % 1000);
    // }

    // {
    //     ScopedTimer timer;

    //     for (int i = 0; i < 1000000; i++) {
    //         book.getMatching().checkLimitOrder(
    //             LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, buyPrices[i]});

    //         book.getMatching().checkLimitOrder(
    //             LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, sellPrices[i]});
    //     }
    // }

    //Insertion Only, Elapsed Time: 0.345704s
    //Book Growth Benchmark
    // std::vector<double> buyPrices;

    // buyPrices.reserve(1000000);

    // for (int i = 0; i < 1000000; i++) {
    //     buyPrices.push_back(rand() % 1000);
    // }

    // {
    //     ScopedTimer timer;

    //     for (int i = 0; i < 1000000; i++) {
    //         book.getMatching().checkLimitOrder(
    //             LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, buyPrices[i]});
    //     }

    // }

    //Cancellation
    // std::vector<double> buyPrices;
    // std::vector<int> ids;

    // buyPrices.reserve(1000000);
    // ids.reserve(1000000);

    // for (int i = 0; i < 1000000; i++) {
    //     buyPrices.push_back(rand() % 1000);
    // }

    // for (int i = 0; i < 1000000; i++) {
    //     book.getMatching().checkLimitOrder(
    //         LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, buyPrices[i]});
    // }

    // for (const auto& pair : book.getIds().idStore){
    //     ids.push_back(pair.first);
    // }

    // std::cout << "Total IDs stored in vector: " << ids.size() << std::endl;
    // std::cout << "Total IDs stored in map: " << book.getIds().idStore.size() << std::endl;
    // std::cout << RAND_MAX << std::endl;

    // {   
    //     ScopedTimer timer;

    //     for (int i = 0; i < 1000000; i++) {
    //         book.getCancel().cancelOrder(ids[i]);
    //     }
    // }

    //Deep Book BenchMark
    // std::vector<double> buyPrices;
    // std::vector<double> sellPrices;

    // buyPrices.reserve(1000000);
    // sellPrices.reserve(1000000);

    // for (int i = 0; i < 1000000; i++) {
    //     buyPrices.push_back(rand() % 1000);
    //     sellPrices.push_back(rand() % 1000);
    // }

    // {
    //     ScopedTimer timer;

    //     for (int i = 0; i < 1000000; i++) {
    //         book.getMatching().checkLimitOrder(
    //             LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, buyPrices[i]});

    //     }

    //     for (int i = 0; i < 1000000; i++){
    //         book.getMatching().checkLimitOrder(
    //             LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, sellPrices[i]});
    //     }
    // }


    //MUCH FASTER THEN RANDOM PRICE LEVEL DEEP BOOK
    //Single Price Level Deep Book

    // {
    //     ScopedTimer timer;

    //     for (int i = 0; i < 1000000; i++) {
    //         book.getMatching().checkLimitOrder(
    //             LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, 500});

    //     }

    //     for (int i = 0; i < 1000000; i++){
    //         book.getMatching().checkLimitOrder(
    //             LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, 500});
    //     }
    // }

    //Price Level Deep Book
    
    {
        ScopedTimer timer;

        for (int i = 0; i < 1000000; i++) {
            book.getMatching().checkLimitOrder(
                LimitOrder{1, OrderType::LIMIT, Side::SELL, 100, static_cast<double>(500 + (rand() % 11) - 5)});

        }

        ;

        for (int i = 0; i < 1000000; i++){
            book.getMatching().checkLimitOrder(
                LimitOrder{1, OrderType::LIMIT, Side::BUY, 100, static_cast<double>(500 + (rand() % 11) - 5)});
        }
    }

    std::cout<< "Matchable Asks Call: " << book.getMatching().hasMatchableAskCalls << "\n";
    std::cout<< "Best Asks Calls: " << book.getMatching().bestAskCalls<< "\n";
    std::cout<< "Get Sell Head Calls: " << book.getMatching().getHeadCalls<< "\n";



}