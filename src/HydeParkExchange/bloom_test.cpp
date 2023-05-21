#include <iostream>
#include <string>
#include "bloom_filter.hpp"

using namespace std;

int main() {
    BloomFilter<string> securityFilter(1000, 3);
    securityFilter.insert("AAPL");
    securityFilter.insert("MSFT");
    securityFilter.insert("GOOGL");
    cout << "Contains AAPL? " << (securityFilter.mayContain("AAPL") ? "Maybe" : "No") << endl;
    cout << "Contains TSLA? " << (securityFilter.mayContain("TSLA") ? "Maybe" : "No") << endl;

    BloomFilter<int> orderIdFilter(5000, 6);
    orderIdFilter.insert(10294);
    orderIdFilter.insert(12323);
    orderIdFilter.insert(79389);
    cout << "Contains 12323? " << (orderIdFilter.mayContain(12323) ? "Maybe" : "No") << endl;

    return 0;
}


