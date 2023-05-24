
#include "single_asset_book.h"
#include "traders.h"
#include <iostream>
#include <memory>
#include <string>
#include <thread>

using namespace hpx;
using namespace std;


void trade(single_asset_book & exchange) {
	jthread t0(start_exchange_thread, ref(exchange));
	jthread t01(consumer_all_orders, ref(exchange));
	jthread t1(send_orders_1, ref(exchange));
	jthread t2(send_orders_2);
	jthread t3(send_orders_3);
}

int main()
{
	single_asset_book exchange = single_asset_book();
	trade(exchange);
	std::this_thread::sleep_for(milliseconds(100));
	exchange.output_market();

	return 0;

}

