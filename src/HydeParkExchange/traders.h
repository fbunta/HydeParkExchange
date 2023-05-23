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
using std::chrono::seconds;
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
using hpx::TradingEntity;

using order_factory = abstract_factory<market_order(OrderSide, int, TradingEntity),
	limit_order(OrderSide, int, float, TradingEntity),
	ioc_order(OrderSide, int, float, TradingEntity)>;
using concrete_order_factory = concrete_factory<order_factory,
	market_order,
	limit_order,
	ioc_order>;

namespace hpx {

	void start_exchange_thread(single_asset_book& q) {
		q.find_fills();
	}

	void send_orders_1(single_asset_book& q) {
		unique_ptr<order_factory> factory(make_unique<concrete_order_factory>());

		unique_ptr<order> order_1(factory->create<limit_order>(OrderSide::Buy, 4, 12.0, TradingEntity::Belvedere));
		cout << "Belvedere placing order" << endl;
		q.insert(order_1);
		
		std::this_thread::sleep_for(milliseconds(30));

		unique_ptr<order> order_2(factory->create<limit_order>(OrderSide::Buy, 2, 12.1, TradingEntity::Belvedere));
		cout << "Belvedere placing order"<< endl;
		q.insert(order_2);
	}

	void send_orders_2(single_asset_book& q) {
		unique_ptr<order_factory> factory(make_unique<concrete_order_factory>());

		unique_ptr<order> order_1(factory->create<limit_order>(OrderSide::Buy, 5, 12.0, TradingEntity::Wolverine));
		cout << "Wolverine placing order" << endl;
		q.insert(order_1);
		
		std::this_thread::sleep_for(milliseconds(20));

		unique_ptr<order> order_2(factory->create<limit_order>(OrderSide::Buy, 3, 12.1, TradingEntity::Wolverine));
		cout << "Wolverine placing order" << endl;
		q.insert(order_2);
	}

	void send_orders_3(single_asset_book& q) {
		unique_ptr<order_factory> factory(make_unique<concrete_order_factory>());
		std::this_thread::sleep_for(milliseconds(40));

		unique_ptr<order> order_1(factory->create<limit_order>(OrderSide::Sell, 1, 12.1, TradingEntity::Citadel));
		cout << "Citadel placing order" << endl;
		q.insert(order_1);
	}
}
#endif