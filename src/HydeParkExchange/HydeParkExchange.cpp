
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
	jthread e0(start_exchange_thread, ref(exchange));
	jthread e1(start_exchange_listener, ref(exchange));
	jthread c1(consumer_all_orders, ref(exchange));

	jthread t1(send_orders_1, ref(exchange));
	jthread t2(send_orders_2);
	jthread t3(send_orders_3);

	// Fix Parsing
	jthread t4(send_orders_4);
}

void test_stream() {
	using order_factory = abstract_factory<market_order(order_side, int, trading_entity),
		limit_order(order_side, int, double, trading_entity),
		ioc_order(order_side, int, double, trading_entity)>;
	using concrete_order_factory = concrete_factory<order_factory,
		market_order,
		limit_order,
		ioc_order>;
	unique_ptr<order_factory> factory(make_unique<concrete_order_factory>());

	unique_ptr<order> order_1(factory->create<limit_order>(buy_order_side, 4, 12.0, trading_entity::Belvedere));
	unique_ptr<order> order_2(factory->create<limit_order>(sell_order_side, 4, 12.0, trading_entity::Wolverine));

	//order* o = order_1.get();
	entity_stream myout(std::cout, trading_entity::Belvedere);
	mymanip(myout, order_1.get());
	mymanip(myout, order_2.get());
}

int main()
{
	single_asset_book exchange = single_asset_book();
	trade(exchange);
	std::this_thread::sleep_for(milliseconds(100));
	exchange.output_market();
}

