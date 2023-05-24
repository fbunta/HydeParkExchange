#ifndef BTREE_h
#  define BTREE_h
#include"level_queue.h"
#include"order.h"
#include<memory>
#include<tuple>
#include<utility>
#include <mutex>
#include <optional>
#include <cmath>

using std::unique_ptr;
using std::make_unique;
using std::optional;
using std::tuple;
using std::lock_guard;
using std::fabs;

namespace hpx {

	class level_btree
	{
	public:
		// this btree will have branches allocated at initialization so that order handling is faster
		// lets assume the last settlement price for this asset is 12.3 and the asset ticks in tenths of a USD
		level_btree() {
			root = make_unique<level_queue>(12.3);
			insert(12.3 + 0.4, root);
			insert(12.3 + 0.2, root);
			insert(12.3 + 0.6, root);
			insert(12.3 + 0.1, root);
			insert(12.3 + 0.3, root);
			insert(12.3 + 0.5, root);
			insert(12.3 + 0.7, root);
			insert(12.3 - 0.4, root);
			insert(12.3 - 0.2, root);
			insert(12.3 - 0.6, root);
			insert(12.3 - 0.1, root);
			insert(12.3 - 0.3, root);
			insert(12.3 - 0.5, root);
			insert(12.3 - 0.7, root);
			// definetly a more math correct way to do this but I feel its within our rights to contain the
			// market like real exchanges with limit up and limit down, this avoids locking on tree operations
		}
		
		level_btree(level_btree const& other) {
			if (other.root) {
				root = make_unique<level_queue>(*other.root);
			}
		}
		
		level_btree& operator=(level_btree const& other) {
			if (&other != this)
				if (other.root) {
					root = make_unique<level_queue>(*other.root);
				}
				else {
					root.reset();
				}
			return *this;
		}
		
		level_btree(level_btree&& other) = default;
		
		void operator=(level_btree&& other) noexcept { 
			swap(root, other.root);
		}

		void insert(order& incoming_order) {
			unique_ptr<order> ord = std::make_unique<order>(incoming_order);
			insert(move(ord), root);
		}
		
		void cancel(int order_id, double price, OrderSide side) {
			optional<level_queue*> level_q = search(price, root.get());
			if (level_q) {
				if (side == OrderSide::Buy) {
					level_q.value()->buy_pop(order_id);
				}
				else {
					level_q.value()->sell_pop(order_id);
				}
				cout << "cancelled " << order_id << endl;
			}
		}

		// lowest price willing to sell
		level_queue* get_best_offer() {
			return in_order(root);
		}

		// highest price willing to buy
		level_queue* get_best_bid() {
			return reverse_in_order(root);
		}

		//TODO WRITE A FUNCTION THAT FINDS BEST BID AND OFFER QUEUES WITH SIZE


	private:
		void insert(unique_ptr<order> incoming_order, unique_ptr<level_queue>& leaf) {
			if (fabs(incoming_order->price_ - leaf->price) < 0.09)
			{
				leaf->push(move(incoming_order));
			}
			else if (incoming_order->price_ > leaf->price)
			{
				if (leaf->right) {
					insert(move(incoming_order), leaf->right);
				}
				else {
					// outside price bounds
					incoming_order->status_ = OrderStatus::Rejected;
				}
			}
			else { // price < left.price_
				if (leaf->left) {
					insert(move(incoming_order), leaf->left);
				}
				else {
					// outside price bounds
					incoming_order->status_ = OrderStatus::Rejected;
				}
			}
		}

		optional<level_queue*> search(double price, level_queue* const leaf) const {
			if (fabs(price - leaf->price) < 0.09) {
				return std::make_optional<level_queue*>(leaf);
			}
			else if (price < leaf->price) {
				return search(price, leaf->left.get());
			}
			else { // price > leaf->price
				return search(price, leaf->right.get());
			}
		}

		level_queue* const in_order(unique_ptr<level_queue>& leaf) {
			if (leaf->left) {
				level_queue* lq = in_order(leaf->left);
				if (lq) {
					return lq;
				}
			}
			if (leaf->sell_size != 0) {
				return leaf.get();
			}
			if (leaf->right) {
				level_queue* lq = in_order(leaf->right);
				if (lq) {
					return lq;
				}
			}
		}

		level_queue* const reverse_in_order(unique_ptr<level_queue>& leaf) {
			if (leaf->right) {
				level_queue* lq = reverse_in_order(leaf->right);
				if (lq) {
					return lq;
				}
			}
			if (leaf->buy_size != 0) {
				return leaf.get();
			}
			if (leaf->left) {
				level_queue* lq = reverse_in_order(leaf->left);
				if (lq) {
					return lq;
				}
			}
		}

		// just used for initialization
		void insert(double price, unique_ptr<level_queue>& leaf) {
			if (price < leaf->price)
			{
				if (leaf->left) {
					insert(price, leaf->left);
				}
				else {
					leaf->left = make_unique<level_queue>(price);
				}
			}
			else if (price > leaf->price)
			{
				if (leaf->right) {
					insert(price, leaf->right);
				}
				else {
					leaf->right = make_unique<level_queue>(price);
				}
			}
		}

		unique_ptr<level_queue> root;
	};
}
#endif