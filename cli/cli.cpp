#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "data/order.h"
#include "storage/nodeStorage.h"
#include "storage/priceStorage.h"
#include "matching/matching.h"

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

    // else{
    //     cout << "Invalid Order Type";
    // }
}

Side parseSide(string sideIn){
    
    
    if (sideIn == "BUY"){
        Side side = Side::BUY;
        return side;
    }
    else if (sideIn == "SELL"){
        Side side = Side::SELL;
        return side;
    }

    // else{
    //     cout << "Invalid Side";
    // }

    // std::cout << "PARSED SIDE" << side;
    // return side;
}

int main() {

    string S;
    getline(cin, S);

    NodeStorage nodes;
    priceStorage prices(nodes);
    Matching match(nodes, prices);

    if (S == "1"){

        //TEMP TRUE WHILE LOOP JUST TO GET IT RUNNING
        while (true){  
            string S, T;
            std::vector<string> tokens;
            getline(cin, S);

            stringstream stringStream(S);

            while (getline(stringStream, T, ' '))
                tokens.push_back(T);

            OrderType type = parseOrderType(tokens[0]);
            Side side = parseSide(tokens[1]);
            
            match.doMatching(type, side, stoi(tokens[2]), stoi(tokens[3]));

        }
    }
    
    return 0;
}

