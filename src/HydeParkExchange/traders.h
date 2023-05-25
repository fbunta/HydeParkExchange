#ifndef TRADERS_H
#  define TRADERS_H
#include "abstract_order_factory.h"
#include "btree.h"
#include "fix_parser.hpp"
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

	int place_limit_order(unique_ptr<order_factory>& factory, OrderSide side,
		int qty, double price, TradingEntity entity)
	{
		unique_ptr<order> order_1(factory->create<limit_order>(side, qty, price, entity));
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
			if (data.order_id_==9) {
				break;
			}
		}
	}

	void start_exchange_thread(single_asset_book& q) {
		q.producer_fills();
	}

	void start_exchange_listener(single_asset_book& q) {
		q.consumer_fills();
	}

	void send_orders_1(single_asset_book& q) {
		unique_ptr<order_factory> factory(make_unique<concrete_order_factory>());
		place_limit_order(factory, OrderSide::Buy, 4, 12.0, TradingEntity::Belvedere);
		place_limit_order(factory, OrderSide::Sell, 4, 12.3, TradingEntity::Belvedere);
		std::this_thread::sleep_for(milliseconds(30));
		int order_id = place_limit_order(factory, OrderSide::Buy, 2, 12.1, TradingEntity::Belvedere);
		std::this_thread::sleep_for(milliseconds(10));
		q.cancel(order_id, 12.1, OrderSide::Buy);
	}

	void send_orders_2() {
		unique_ptr<order_factory> factory(make_unique<concrete_order_factory>());
		place_limit_order(factory, OrderSide::Buy, 5, 12.0, TradingEntity::Wolverine);
		place_limit_order(factory, OrderSide::Sell, 7, 12.3, TradingEntity::Wolverine);
		std::this_thread::sleep_for(milliseconds(20));
		place_limit_order(factory, OrderSide::Buy, 3, 12.1, TradingEntity::Wolverine);
	}

	void send_orders_3() {
		unique_ptr<order_factory> factory(make_unique<concrete_order_factory>());
		std::this_thread::sleep_for(milliseconds(40));
		place_limit_order(factory, OrderSide::Sell, 1, 12.1, TradingEntity::Citadel);
		place_limit_order(factory, OrderSide::Sell, 10, 12.2, TradingEntity::Citadel);
		place_limit_order(factory, OrderSide::Sell, 15, 12.3, TradingEntity::Citadel);
		place_limit_order(factory, OrderSide::Sell, 20, 12.4, TradingEntity::Citadel);
	}

	void send_orders_4() {
		unique_ptr<order_factory> factory(make_unique<concrete_order_factory>());
		std::this_thread::sleep_for(milliseconds(60));
		// Simulate orders from an input stream in real-world formatting

		std::string_view fix_1 = "8=FIX.4.29=035=D49=83019956=AZKJ34=057=362052=20150406-12:17:2711=0c968e69-c3ff-4f9f-bc66-9e5ebccd980741=e0568b5c-8bb1-41f0-97bf-5eed32828c241=90964630055=SJM48=46428843022=154=138=7570040=115=USD59=060=20150406-12:17:278201=1207=P10=0";
		//std::string_view fix_2 = "8=FIX.4.29=035=D49=AZKJ56=JGEB34=150=362057=946152=20150406-12:17:2711=3a074d1d-fb06-4eb0-b2f8-0912c5735f65109=8301991=90964630055=SJM48=46428843022=154=138=7570040=115=USD59=08011=0c968e69-c3ff-4f9f-bc66-9e5ebccd980760=20150406-12:17:278201=3207=P10=0";
		//std::string_view fix_3 = "8=FIX.4.29=035=D49=AZKJ56=JGEB34=250=362057=946152=20150406-12:17:2711=3a074d1d-fb06-4eb0-b2f8-0912c5735f6541=335844d8-fc05-41d9-825b-6f3a5059a29b109=8301991=AZKJ90964630055=SJM48=46428843022=154=138=7570040=115=USD59=060=20150406-12:17:278201=1207=P10=0";
		
		std::optional<std::tuple<double, double, bool, std::string>> result = parseFixMessage(fix_1);
		if (result) {
			auto [price, quantity, is_buy, symbol] = result.value();
			if(is_buy){
				place_limit_order(factory, OrderSide::Buy, quantity, price, TradingEntity::IMC);
			}
			else{
				place_limit_order(factory, OrderSide::Sell, quantity, price, TradingEntity::IMC);
			}
		
		}
		
		//place_limit_order(factory, OrderSide::Sell, 10, 12.2, TradingEntity::Citadel);
		//place_limit_order(factory, OrderSide::Sell, 15, 12.3, TradingEntity::Citadel);
		//place_limit_order(factory, OrderSide::Sell, 20, 12.4, TradingEntity::Citadel);
	}
	
}
#endif