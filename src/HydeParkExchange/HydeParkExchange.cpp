
#include "abstract_order_factory.h"
#include "order.h"
#include "lock_queue.h"
#include "traders.h"
#include <iostream>
#include <memory>
#include <string>
#include <thread>

using namespace hpx;
using namespace std;

using order_factory = abstract_factory<market_order(OrderSide, int),
	limit_order(OrderSide, int, float),
	ioc_order(OrderSide, int, float)>;
using concrete_order_factory = concrete_factory<order_factory,
	market_order,
	limit_order,
	ioc_order>;

int main()
{
	unique_ptr<order_factory> factory(make_unique<concrete_order_factory>());

	auto l(factory->create<market_order>(OrderSide::Buy, 10));

	ex_queue exchange = ex_queue<int>();
	exchange.push(0);

	jthread t1(send_orders_1, ref(exchange));
	jthread t2(send_orders_2, ref(exchange));
	jthread t3(pop_orders_1, ref(exchange));
	t1.join();
	t2.join();
	t3.join();

	return 0;

}

