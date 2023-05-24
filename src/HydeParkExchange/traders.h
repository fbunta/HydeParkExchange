#ifndef TRADERS_H
#  define TRADERS_H
#include "abstract_order_factory.h"
#include "btree.h"
#include "order.h"
#include <iostream>
#include <chrono>
#include <utility>
#include <memory>
#include <mutex>
#include <queue>

using std::move;
using std::condition_variable;
using std::queue;
using std::cout;
using std::endl;
using std::chrono::seconds;
using std::chrono::milliseconds;
using std::unique_ptr;
using std::make_unique;
using std::mutex;
using std::unique_lock;
using hpx::order;
using hpx::ioc_order;
using hpx::market_order;
using hpx::limit_order;
using hpx::abstract_factory;
using hpx::concrete_factory;
using hpx::OrderSide;
using hpx::TradingEntity;

using order_factory = abstract_factory<market_order(OrderSide, int, TradingEntity),
	limit_order(OrderSide, int, double, TradingEntity),
	ioc_order(OrderSide, int, double, TradingEntity)>;
using concrete_order_factory = concrete_factory<order_factory,
	market_order,
	limit_order,
	ioc_order>;

namespace hpx {

	mutex m;
	queue<order> data_queue;
	condition_variable cond;

	int place_limit_order(unique_ptr<order_factory>& factory, OrderSide side, int qty, double price, TradingEntity entity)
	{
		unique_ptr<order> order_1(factory->create<limit_order>(side, qty, price, entity));
		cout << "placing order" << endl;
		lock_guard lk(m);
		data_queue.push(*order_1);
		cond.notify_one();
		return order_1->order_id_;
	}

	void consumer_all_orders(single_asset_book& q)
	{
		while (true) {
			unique_lock<mutex> lk(m);
			cond.wait(lk, [] {return !data_queue.empty();});
			order data = move(data_queue.front());
			data_queue.pop();
			lk.unlock();
			q.insert(data);
			if (data.order_id_==4) {
				break;
			}
		}
	}

	void start_exchange_thread(single_asset_book& q) {
		q.find_fills();
	}

	void send_orders_1(single_asset_book& q) {
		unique_ptr<order_factory> factory(make_unique<concrete_order_factory>());
		place_limit_order(factory, OrderSide::Buy, 4, 12.0, TradingEntity::Belvedere);	
		std::this_thread::sleep_for(milliseconds(30));
		int order_id = place_limit_order(factory, OrderSide::Buy, 2, 12.1, TradingEntity::Belvedere);
		std::this_thread::sleep_for(milliseconds(10));
		q.cancel(order_id, 12.1);
	}

	void send_orders_2() {
		unique_ptr<order_factory> factory(make_unique<concrete_order_factory>());
		place_limit_order(factory, OrderSide::Buy, 5, 12.0, TradingEntity::Wolverine);
		std::this_thread::sleep_for(milliseconds(20));
		place_limit_order(factory, OrderSide::Buy, 3, 12.1, TradingEntity::Wolverine);
	}

	void send_orders_3() {
		unique_ptr<order_factory> factory(make_unique<concrete_order_factory>());
		std::this_thread::sleep_for(milliseconds(40));
		place_limit_order(factory, OrderSide::Sell, 1, 12.1, TradingEntity::Citadel);
	}
}
#endif