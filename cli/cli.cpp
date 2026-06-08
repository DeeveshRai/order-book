#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "storage/nodeStorage.h"
#include "storage/priceStorage.h"
#include "matching/matching.h"
#include "cancel/cancel.h"
#include "storage/idMap.h"

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

    //Temp condition
    while (true){

        string S;
        getline(cin, S);

        if (S == "1"){

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

        //Cancel loop
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
        }
    }
    
    
    return 0;
}

