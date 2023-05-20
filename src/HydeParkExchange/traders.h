#ifndef TRADERS_H
#  define TRADERS_H
#include "abstract_order_factory.h"
#include "btree.h"
#include "order.h"
#include <iostream>
#include <chrono>
#include <utility>
#include <memory>

using std::cout;
using std::endl;
using std::chrono::milliseconds;
using std::unique_ptr;
using std::make_unique;
using hpx::order;
using hpx::ioc_order;
using hpx::market_order;
using hpx::limit_order;
using hpx::abstract_factory;
using hpx::concrete_factory;
using hpx::OrderSide;

using order_factory = abstract_factory<market_order(OrderSide, int),
	limit_order(OrderSide, int, float),
	ioc_order(OrderSide, int, float)>;
using concrete_order_factory = concrete_factory<order_factory,
	market_order,
	limit_order,
	ioc_order>;

namespace hpx {

	void send_orders_1(level_btree& q) {
		unique_ptr<order_factory> factory(make_unique<concrete_order_factory>());

		unique_ptr<order> order_1(factory->create<ioc_order>(OrderSide::Buy, 1, 12.3));
		q.insert(*order_1);

		unique_ptr<order> order_2(factory->create<ioc_order>(OrderSide::Buy, 3, 12.3));
		q.insert(*order_2);

		unique_ptr<order> order_3(factory->create<ioc_order>(OrderSide::Buy, 10, 12.2));
		q.insert(*order_3);

		unique_ptr<order> order_4(factory->create<ioc_order>(OrderSide::Buy, 2, 12.3));
		q.insert(*order_4);

		unique_ptr<order> order_5(factory->create<ioc_order>(OrderSide::Buy, 12, 12.2));
		q.insert(*order_5);

		unique_ptr<order> order_6(factory->create<ioc_order>(OrderSide::Buy, 4, 12.3));
		q.insert(*order_6);

	}

	void send_orders_2(level_btree& q) {
		unique_ptr<order_factory> factory(make_unique<concrete_order_factory>());

		unique_ptr<order> order_1(factory->create<ioc_order>(OrderSide::Buy, 1, 12.3));
		q.insert(*order_1);

		unique_ptr<order> order_2(factory->create<ioc_order>(OrderSide::Buy, 3, 12.3));
		q.insert(*order_2);

		unique_ptr<order> order_3(factory->create<ioc_order>(OrderSide::Buy, 10, 12.2));
		q.insert(*order_3);

		unique_ptr<order> order_4(factory->create<ioc_order>(OrderSide::Buy, 2, 12.3));
		q.insert(*order_4);

		unique_ptr<order> order_5(factory->create<ioc_order>(OrderSide::Buy, 12, 12.2));
		q.insert(*order_5);

		unique_ptr<order> order_6(factory->create<ioc_order>(OrderSide::Buy, 4, 12.3));
		q.insert(*order_6);

	}

	//void pop_orders_1(level_btree& q) {
	//	std::this_thread::sleep_for(milliseconds(40));
	//	cout << q.pop() << endl;
	//	cout << q.pop() << endl;
	//	cout << q.pop() << endl;
	//	cout << q.pop() << endl;
	//	cout << q.pop() << endl;
	//	cout << q.pop() << endl;
	//	cout << q.pop() << endl;
	//	cout << q.pop() << endl;
	//	cout << q.pop() << endl;
	//	cout << q.pop() << endl;
	//	cout << q.pop() << endl;
	//	cout << q.pop() << endl;
	//}
}
#endif