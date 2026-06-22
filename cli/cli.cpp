#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "storage/nodeStorage.h"
#include "storage/priceStorage.h"
#include "matching/matching.h"
#include "cancel/cancel.h"
#include "storage/idMap.h"
#include <ctime>

using namespace std;

//TODO: Need to refactor CLI so its cleaner (more robust with exception handling and custom messages),
// and announced when orders have been added, filled (partial or full) and cancelled

//Instead of doing cancellation and orders in different "modes" try and make it so its arg based instaed

OrderType parseOrderType(string orderType){
    if (orderType == "LIMIT"){
        OrderType type = OrderType::LIMIT;
        return type;
    }
    else if (orderType == "MARKET"){
        OrderType type = OrderType::MARKET;
        return type;
    }

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

}

int main() {

    NodeStorage nodes;
    priceStorage prices(nodes);
    idMap ids(nodes);
    Matching match(nodes, prices, ids);
    Cancel cancel(nodes, prices, ids);

    std::cout << "Order Book Start... \n" << "Select a mode: \n" << "1. Orders\n" << "2. Cancellation\n" ;
    //Temp condition
    while (true){
        
        string S;
        getline(cin, S);

        if (S == "1"){

            std::cout << "Enter Order: \n";
            //TEMP TRUE WHILE LOOP JUST TO GET IT RUNNING
            while (true){  
                string S, T;
                std::vector<string> tokens;
                getline(cin, S);

                if (S == "exit"){
                    break;
                }

                stringstream stringStream(S);

                while (getline(stringStream, T, ' '))
                    tokens.push_back(T);
                
                //MARKET ORDERS BROKEN HERE -> Market orders only have 3 MARKET SIDE QTY
                OrderType type = parseOrderType(tokens[0]);
                Side side = parseSide(tokens[1]);

                if (type == OrderType::LIMIT){
                    match.doLimitMatching(type, side, stoi(tokens[2]), stoi(tokens[3]));
                }

                else if (type == OrderType::MARKET){
                    match.doMarketMatching(type, side, stoi(tokens[2]));
                }
                

                

            }
        }

        //Cancel loop - Make some sort of annocunemtn that cancel passed
        else if (S == "2"){
            string S, T;
            std::vector<string> ids;
            getline(cin, S);
            stringstream stringStream(S);
            while (getline(stringStream, T, ' ')){
                ids.push_back(T);
            }

            string cancellation = ids[0];
            int id = stoi(ids[1]);
            cancel.cancelOrder(id);

        }

        else if (S == "dump"){
            for (auto& node : nodes.store){
                std::cout << "node id: " << node.id << "\n" << " node price: " << node.price << "\n" << " node qty: " << node.qty << "\n" << " node prevIndex: " << node.prevIndex <<
                "\n" << " node nextIndex: " << node.nextIndex << "\n" << " node side: " << node.side << "\n" << "node type: "<< node.type << "\n" << "node active?" << node.active << "\n"; 
            }

            if (prices.buyPriceLevelStorage.empty()){
                std::cout << "buy empty" << "\n";
            }

            for (auto& orderNode : prices.buyPriceLevelStorage){
                std::cout << " Buy Price: " << orderNode.first << "\n" << " Head: " <<  orderNode.second[0] << "\n" << " Tail: " << orderNode.second[1] << "\n";   
            }

            if (prices.sellPriceLevelStorage.empty()){
                std::cout << "sell empty" << "\n";
            }

            for (auto& orderNode : prices.sellPriceLevelStorage){
                std::cout << " Sell Price: " << orderNode.first << "\n" << " Head: " <<  orderNode.second[0] << "\n" << " Tail: " << orderNode.second[1] << "\n";    
            }

            for (auto& id : ids.idStore){
                std::cout << " ids: " << id.first << "\n" << "index" << id.second << "\n";
            }
        }
    }
    
    
    return 0;
}

