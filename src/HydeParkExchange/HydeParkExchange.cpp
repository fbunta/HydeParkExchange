
#include "single_asset_book.h"
#include "traders.h"
#include "entity_stream.h"
#include <iostream>
#include <memory>
#include <string>
#include <thread>

using namespace hpx;
using namespace std;


void trade(single_asset_book & exchange) {
	// spin up the exchange
	jthread e0(start_exchange_thread, ref(exchange));
	jthread e1(start_fill_listener, ref(exchange), trading_entity::Wolverine);
	jthread c1(consumer_all_orders, ref(exchange));

	// spin up the trading sessions
	jthread t1(send_orders_1, ref(exchange));
	jthread t2(send_orders_2);
	jthread t3(send_orders_3);
}

int main()
{
	single_asset_book exchange = single_asset_book();
	trade(exchange);
	std::this_thread::sleep_for(100ms);
	exchange.output_market();
}

