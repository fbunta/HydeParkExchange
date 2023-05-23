#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <unordered_map>
#include<sstream>
#include <fstream>

std::unordered_map<int, std::string> getFieldNames() {
  std::unordered_map<int, std::string> fieldNames;

  fieldNames[1] = "Account";
  fieldNames[6] = "AvgPx";
  fieldNames[8] = "BeginString";
  fieldNames[10] = "Checksum";
  fieldNames[11] = "ClOrdID";
  fieldNames[14] = "CumQty";
  fieldNames[15] = "Currency";
  fieldNames[17] = "ExecID";
  fieldNames[20] = "ExecTransType";
  fieldNames[22] = "IDSource";
  fieldNames[29] = "LastQty";
  fieldNames[30] = "LastMkt";
  fieldNames[31] = "LastPx";
  fieldNames[32] = "LastShares";
  fieldNames[34] = "MsgSeqNum";
  fieldNames[35] = "MessageType"; 
  fieldNames[37] = "OrderID";
  fieldNames[38] = "OrderQty";
  fieldNames[39] = "OrdStatus";
  fieldNames[40] = "OrdType";
  fieldNames[41] = "OrigClOrdID";
  fieldNames[44] = "Price"; 
  fieldNames[48] = "SecurityID";
  fieldNames[49] = "SenderCompID";
  fieldNames[50] = "SenderSubID";
  fieldNames[52] = "SendingTime";
  fieldNames[54] = "Side";
  fieldNames[55] = "Symbol";
  fieldNames[56] = "TargetCompID";
  fieldNames[57] = "TargetSubID";
  fieldNames[59] = "TimeInForce";
  fieldNames[60] = "TransactTime";
  fieldNames[75] = "TradeDate";
  fieldNames[76] = "TrdMatchID";
  fieldNames[109] = "ClientID";
  fieldNames[150] = "ExecType";
  fieldNames[151] = "LeavesQty";
  fieldNames[207] = "SecurityExchange";
  fieldNames[8201] = "Field1";

  return fieldNames;
}

// Function to parse a FIX message using regex
void parseFixMessage(const std::string &fixMessage) {
  std::unordered_map<int, std::string> fieldNames = getFieldNames();

  // note the \x01 is an SOH character (some people visualize with a pipe character)
  std::regex pattern("\x01([^=]+)=([^\x01]+)");

  std::sregex_iterator it(fixMessage.begin(), fixMessage.end(), pattern);
  std::sregex_iterator end;

  while (it != end) {
    std::smatch match = *it;
    std::string tag = match.str(1);
    std::string value = match.str(2);
    std::string fieldName = fieldNames[std::stoi(tag)];

    std::cout << fieldName << "=" << value << std::endl;
    it++;
  }
}

int main_()
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

    return 0;
}