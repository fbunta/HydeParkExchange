#ifndef TRADERS_H
#  define TRADERS_H
#include "abstract_order_factory.h"
#include "btree.h"
#include "fix_parser.hpp"
#include "order.h"
#include "single_asset_book.h"
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
using hpx::single_asset_book;
using hpx::ioc_order;
using hpx::market_order;
using hpx::limit_order;
using hpx::abstract_factory;
using hpx::concrete_factory;
using hpx::order_side;
using hpx::b;
using hpx::s;
using hpx::buy_order_side;
using hpx::sell_order_side;
using hpx::trading_entity;

using order_factory = abstract_factory<market_order(order_side, int, trading_entity),
	limit_order(order_side, int, double, trading_entity),
	ioc_order(order_side, int, double, trading_entity)>;
using concrete_order_factory = concrete_factory<order_factory,
	market_order,
	limit_order,
	ioc_order>;

using namespace std::chrono_literals;

namespace hpx {

	mutex m;
	queue<order> data_queue;
	condition_variable cond;

	int place_limit_order(unique_ptr<order_factory>& factory, order_side side,
		int qty, double price, trading_entity entity)
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

	void start_fill_listener(single_asset_book& q, trading_entity entity) {
		q.consumer_fills(entity);
	}

	int send_fix_order(std::string_view fix, trading_entity t){
		unique_ptr<order_factory> factory(make_unique<concrete_order_factory>());
		std::optional<std::tuple<double, double, bool, std::string>> result = parseFixMessage(fix);
		if (result) {
			//auto [price, quantity, is_buy, symbol] = result.value();
			std::tuple<double, int, bool, std::string> res = result.value();

			// Specifying return values using structured bindings
			double price;
			double quantity;
			bool is_buy;
			std::string symbol;
			std::tie(price, quantity, is_buy, symbol) = res;

			if(is_buy){
				int order_id = place_limit_order(factory, buy_order_side, quantity, price, trading_entity::IMC);
				return order_id;
			}
			else{
				int order_id = place_limit_order(factory, sell_order_side, quantity, price, trading_entity::IMC);
				return order_id;
			}
			
		}
		return -1;
	}

	void send_orders_1(single_asset_book& q) {

		//Belvedere's Trading
		
		trading_entity Belv = trading_entity::Belvedere;

		std::string_view belv_trade_1 ="8=FIX.4.29=035=D49=83019956=AZKJ34=057=362052=20150406-12:17:2711=0c968e69-c3ff-4f9f-bc66-9e5ebccd980741=e0568b5c-8bb1-41f0-97bf-5eed32828c241=90964630055=SPX44=12.022=154=238=440=115=USD59=060=20150406-12:17:278201=1207=P10=0";
		std::string_view belv_trade_2 ="8=FIX.4.29=035=D49=83019956=AZKJ34=057=362052=20150406-12:17:2711=0c968e69-c3ff-4f9f-bc66-9e5ebccd980741=e0568b5c-8bb1-41f0-97bf-5eed32828c241=90964630055=SPX44=12.322=154=138=440=115=USD59=060=20150406-12:17:278201=1207=P10=0";
		std::string_view belv_trade_3 ="8=FIX.4.29=035=D49=83019956=AZKJ34=057=362052=20150406-12:17:2711=0c968e69-c3ff-4f9f-bc66-9e5ebccd980741=e0568b5c-8bb1-41f0-97bf-5eed32828c241=90964630055=SPX44=12.122=154=238=240=115=USD59=060=20150406-12:17:278201=1207=P10=0";
		
		send_fix_order(belv_trade_1, Belv);
		send_fix_order(belv_trade_2, Belv);

		std::this_thread::sleep_for(30ms);

		int order_id = send_fix_order(belv_trade_3, Belv);

		std::this_thread::sleep_for(10ms);
		q.cancel(order_id, 12.1, buy_order_side);
	}

	void send_orders_2() {
		//Wolverine's Trading 

		trading_entity Wolv = trading_entity::Wolverine;

		std::string_view wolv_trade_1 ="8=FIX.4.29=035=D49=83019956=AZKJ34=057=362052=20150406-12:17:2711=0c968e69-c3ff-4f9f-bc66-9e5ebccd980741=e0568b5c-8bb1-41f0-97bf-5eed32828c241=90964630055=SPX44=12.022=154=238=540=115=USD59=060=20150406-12:17:278201=1207=P10=0";
		std::string_view wolv_trade_2 ="8=FIX.4.29=035=D49=83019956=AZKJ34=057=362052=20150406-12:17:2711=0c968e69-c3ff-4f9f-bc66-9e5ebccd980741=e0568b5c-8bb1-41f0-97bf-5eed32828c241=90964630055=SPX44=12.322=154=138=740=115=USD59=060=20150406-12:17:278201=1207=P10=0";
		std::string_view wolv_trade_3 ="8=FIX.4.29=035=D49=83019956=AZKJ34=057=362052=20150406-12:17:2711=0c968e69-c3ff-4f9f-bc66-9e5ebccd980741=e0568b5c-8bb1-41f0-97bf-5eed32828c241=90964630055=SPX44=12.122=154=238=340=115=USD59=060=20150406-12:17:278201=1207=P10=0";
	

		send_fix_order(wolv_trade_1, Wolv);
		send_fix_order(wolv_trade_2, Wolv);

		std::this_thread::sleep_for(20ms);

		send_fix_order(wolv_trade_3, Wolv);
	}

	void send_orders_3() {
		//Citadel's trading
		trading_entity CitSec = trading_entity::Citadel;
		std::this_thread::sleep_for(40ms);

		std::string_view citadel_trade_2 = "8=FIX.4.29=035=D49=83019956=AZKJ34=057=362052=20150406-12:17:2711=0c968e69-c3ff-4f9f-bc66-9e5ebccd980741=e0568b5c-8bb1-41f0-97bf-5eed32828c241=90964630055=SPX44=12.222=154=138=1040=115=USD59=060=20150406-12:17:278201=1207=P10=0";
		std::string_view citadel_trade_1 = "8=FIX.4.29=035=D49=83019956=AZKJ34=057=362052=20150406-12:17:2711=0c968e69-c3ff-4f9f-bc66-9e5ebccd980741=e0568b5c-8bb1-41f0-97bf-5eed32828c241=90964630055=SPX44=12.122=154=138=140=115=USD59=060=20150406-12:17:278201=1207=P10=0";
		std::string_view citadel_trade_3 = "8=FIX.4.29=035=D49=83019956=AZKJ34=057=362052=20150406-12:17:2711=0c968e69-c3ff-4f9f-bc66-9e5ebccd980741=e0568b5c-8bb1-41f0-97bf-5eed32828c241=90964630055=SPX44=12.322=154=138=1540=115=USD59=060=20150406-12:17:278201=1207=P10=0";
		std::string_view citadel_trade_4 = "8=FIX.4.29=035=D49=83019956=AZKJ34=057=362052=20150406-12:17:2711=0c968e69-c3ff-4f9f-bc66-9e5ebccd980741=e0568b5c-8bb1-41f0-97bf-5eed32828c241=90964630055=SPX44=12.422=154=138=2040=115=USD59=060=20150406-12:17:278201=1207=P10=0";

		send_fix_order(citadel_trade_1, CitSec);
		send_fix_order(citadel_trade_2, CitSec);
		send_fix_order(citadel_trade_3, CitSec);
		send_fix_order(citadel_trade_4, CitSec);

	}
	
}
#endif