#ifndef SINGLE_ASSET_BOOK_H
#	define SINGLE_ASSET_BOOK_H
#include "order.h"
#include "btree.h"
#include <memory>
#include <iostream>
#include <algorithm>
#include <queue>
#include <mutex>
#include <utility>

using std::move;
using std::queue;
using std::condition_variable;
using std::unique_lock;
using std::mutex;
using std::cout;
using std::endl;
using std::unique_ptr;
using std::make_unique;
using hpx::level_btree;
using hpx::order;

namespace hpx {
	class single_asset_book {
	public:
		single_asset_book() {
			tree = make_unique<level_btree>();
		}

		void insert(order order) {
			tree->insert(order);
			cout << order << endl;
		}

		void cancel(int order_id, double price, order_side side) {
			tree->cancel(order_id, price, side);
			cout << "cancelled " << order_id << endl;
		}

		void producer_fills() {
			while (true) {
				level_queue* best_bid_queue = tree->get_best_bid();
				if (best_bid_queue) {
					unique_ptr<fill> f = best_bid_queue->pop();
					if (f != nullptr) {
						lock_guard lk(m);
						data_queue.push(*f);
						cond.notify_one();
						break;
					}
				}
			}
		}

		void consumer_fills()
		{
			while (true) {
				unique_lock<mutex> lk(m);
				cond.wait(lk, [&] {return !data_queue.empty();});
				fill data = move(data_queue.front());
				data_queue.pop();
				lk.unlock();
				cout << data.qty_ << " filled @" << data.price_ << " with orderIds: "
					<< data.buy_order_id_ << " and " << data.sell_order_id_ << endl;
				if (data.sell_order_id_ < 9) {
					break;
				}
			}
		}

		void output_market() {
			level_queue * lq_bid = tree->get_best_bid();
			level_queue* lq_offer = tree->get_best_offer();
			cout << "Best bid price: " << lq_bid->price << " bid size: " << lq_bid->buy_size << endl;
			cout << "Best offer price: " << lq_offer->price << " offer size: " << lq_offer->sell_size << endl;
		}
		

	private:
		unique_ptr<level_btree> tree;
		mutex m;
		condition_variable cond;
		queue<fill> data_queue;
	};
}

#endif