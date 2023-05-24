#ifndef SINGLE_ASSET_BOOK_H
#	define SINGLE_ASSET_BOOK_H
#include "order.h"
#include "btree.h"
#include<memory>
#include<iostream>
#include<algorithm>

using std::cout;
using std::endl;
using std::unique_ptr;
using std::make_unique;
using hpx::level_btree;
using hpx::order;
using std::chrono::milliseconds;

namespace hpx {
	class single_asset_book {
	public:
		single_asset_book() {
			tree = make_unique<level_btree>();
		}

		void insert(order order) {
			tree->insert(order);
		}

		//void insert(unique_ptr<order>&& order) {
		//	tree->insert(move(order));
		//}

		void cancel(int order_id, double price) {
			tree->cancel(order_id, price);
		}

		void find_fills() {
			while (true) {
				level_queue* best_bid_queue = tree->get_best_bid();
				if (best_bid_queue) {
					best_bid_queue->pop();
					break;
				}
				std::this_thread::sleep_for(milliseconds(1));
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
	};
}

#endif