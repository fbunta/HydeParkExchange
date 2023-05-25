#ifndef FIX_PARSER_H
#define FIX_PARSER_H

#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include "ctre.hpp"

static std::unordered_map<int, std::string> fieldNames = {
  {1, "Account"},
  {6, "AvgPx"},
  {8, "BeginString"},
  {10, "Checksum"},
  {11, "ClOrdID"},
  {14, "CumQty"},
  {15, "Currency"},
  {17, "ExecID"},
  {20, "ExecTransType"},
  {22, "IDSource"},
  {29, "LastQty"},
  {30, "LastMkt"},
  {31, "LastPx"},
  {32, "LastShares"},
  {34, "MsgSeqNum"},
  {35, "MessageType"},
  {37, "OrderID"},
  {38, "OrderQty"},
  {39, "OrdStatus"},
  {40, "OrdType"},
  {41, "OrigClOrdID"},
  {44, "Price"},
  {48, "SecurityID"},
  {49, "SenderCompID"},
  {50, "SenderSubID"},
  {52, "SendingTime"},
  {54, "Side"},
  {55, "Symbol"},
  {56, "TargetCompID"},
  {57, "TargetSubID"},
  {59, "TimeInForce"},
  {60, "TransactTime"},
  {75, "TradeDate"},
  {76, "TrdMatchID"},
  {109, "ClientID"},
  {150, "ExecType"},
  {151, "LeavesQty"},
  {207, "SecurityExchange"},
  {8201, "Field1"},
};

std::optional<std::tuple<double, double, bool, std::string>> parseFixMessage(const std::string_view &fixMessage) {
    double px;
    double qty;
    bool side_is_buy;
    std::string symbol;

    bool px_flag, qty_flag, buy_flag, symbol_flag;

    for (auto match : ctre::range<"\x01([^=]+)=([^\x01]+)">(fixMessage)) {
        std::string fieldName = std::string(match.get<1>().to_view());
        int fieldNum = std::stoi(fieldName);
        //std::cout << fieldNames[std::stoi(fieldName)]
        //          << " : " << match.get<2>().to_view() << "\n";
        if(fieldNum == 44){
            std::string px_string = std::string(match.get<2>().to_view());
            px = std::stoi(px_string);
            px_flag = true;
        }
        if(fieldNum == 38){
            std::string px_string = std::string(match.get<2>().to_view());
            qty = std::stoi(px_string);
            qty_flag=true;
        }
        if(fieldNum == 54){
            // note this is because 2 is buy and 1 is sell
            side_is_buy = std::stoi(std::string(match.get<2>().to_view()))-1;
            buy_flag = true;
        }
        if(fieldNum == 55){
            symbol = std::string(match.get<2>().to_view());
            symbol_flag = true;
        }
    }
    if(px_flag && qty_flag && buy_flag && symbol_flag){
        return std::make_tuple(px, qty, side_is_buy, symbol);
    }
    else{
        return std::nullopt;
    }
}

#endif  // FIX_PARSER_H