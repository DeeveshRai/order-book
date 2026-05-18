@echo off
g++ -std=c++17 -I. -w cli/cli.cpp matching/matching.cpp storage/nodeStorage.cpp storage/priceStorage.cpp storage/idMap.cpp cancel/cancel.cpp -o orderbook.exe