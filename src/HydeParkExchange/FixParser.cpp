#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <unordered_map>
#include<sstream>
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

void parseFixMessage(const std::string_view &fixMessage) {
    
    //auto matches = ctre::match<"\x01([^=]+)=([^\x01]+)">(fixMessage);
    for (auto match : ctre::range<"\x01([^=]+)=([^\x01]+)">(fixMessage)) {
        std::string fieldName = std::string(match.get<1>().to_view());
        std::cout << fieldNames[std::stoi(fieldName)]
                  << " : " << match.get<2>().to_view() << "\n";
    }
}

int main()
{
    std::ifstream inputFile("FIX.txt");
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open input file." << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        parseFixMessage(line);
        std::cout << " " << std::endl;
    }
    inputFile.close();
    
    // To test CTRE
    parseFixMessage("8=FIX.4.29=035=D49=83019956=AZKJ34=057=362052=20150406-12:17:2711=0c968e69-c3ff-4f9f-bc66-9e5ebccd980741=e0568b5c-8bb1-41f0-97bf-5eed32828c241=90964630055=SJM48=46428843022=154=138=7570040=115=USD59=060=20150406-12:17:278201=1207=P10=0");

    return 0;
}
