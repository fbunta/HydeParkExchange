#ifndef SINGLE_ASSET_BOOK_H
#	define SINGLE_ASSET_BOOK_H
#include "order.h"
#include "btree.h"

using hpx::level_btree;
using hpx::order;

namespace hpx {
	class single_asset_book {
	public:
		single_asset_book() {
			buy_tree = level_btree();
			sell_tree = level_btree();
		}

		void insert(order order) {
			if (order.side_ == OrderSide::Buy) {
				buy_tree.insert(order);
			}
			else {
				sell_tree.insert(order);
			}
		}

		void cancel(order order) {
			if (order.side_ == OrderSide::Buy) {
				buy_tree.cancel(order.price_, order.order_id_);
			}
			else {
				sell_tree.cancel(order.price_, order.order_id_);
			}
		}
	private:
		level_btree buy_tree;
		level_btree sell_tree;
	};
}

#endif