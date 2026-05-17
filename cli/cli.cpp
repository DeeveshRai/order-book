#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "order.h"
#include "matching.h"

using namespace std;

OrderType parseOrderType(string orderType){
    if (orderType == "LIMIT"){
        OrderType type = OrderType::LIMIT;
        return type;
    }
    else if (orderType == "MARKET"){
        OrderType type = OrderType::MARKET;
        return type;
    }

    else{
        cout << "Invalid Order Type";
    }
}

Side parseSide(string sideIn){
    Side side;
    
    if (sideIn == "BUY"){
        Side side = Side::BUY;
    }
    else if (sideIn == "SELL"){
        Side side = Side::SELL;
    }

    else{
        cout << "Invalid Side";
    }

    return side;
}

int main() {
    string S, T;
    std::vector<string> tokens;
    getline(cin, S);

    stringstream stringStream(S);

    while (getline(stringStream, T, ' '))
        tokens.push_back(T);

    OrderType type = parseOrderType(tokens[0]);
    Side side = parseSide(tokens[1]);

    priceStorage prices;
    NodeStorage nodes;
    Matching match(nodes, prices);
    match.doMatching(type, side, stoi(tokens[2]), stoi(tokens[3]));

    
    return 0;
}

