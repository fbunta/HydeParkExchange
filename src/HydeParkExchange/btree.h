#ifndef BTREE_h
#  define BTREE_h
#include"level_queue.h"
#include"order.h"
#include<memory>
#include<utility>
using std::unique_ptr;
using std::make_unique;

namespace hpx {

	class level_btree
	{
	public:
		level_btree() = default;
		
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
		
		void operator=(level_btree&& other) noexcept { swap(root, other.root); }

		void insert(order incoming_order) {
			if (root) {
				insert(incoming_order, *root);
			}
			else {
				root = make_unique<level_queue>(incoming_order.price_);
				root->push(incoming_order);
			}
		}
		void cancel(float price, int order_id) {
			level_queue* lq = search(price, root.get());
			if (lq != nullptr) {
				lq->pop(order_id);
			}
		}

	private:
		void insert(order incoming_order, level_queue& leaf) {
			if (incoming_order.price_ < leaf.price)
			{
				if (leaf.left) {
					insert(incoming_order, *leaf.left);
				}
				else {
					leaf.left = make_unique<level_queue>(incoming_order.price_);
					leaf.left->push(incoming_order);
				}
			}
			else if (incoming_order.price_ > leaf.price)
			{
				if (leaf.right) {
					insert(incoming_order, *leaf.right);
				}
				else {
					leaf.right = make_unique<level_queue>(incoming_order.price_);
					leaf.right->push(incoming_order);
				}
			}
			else { // price == left.price_
				leaf.push(incoming_order);
			}
		}
		level_queue* search(float price, level_queue* leaf) const {
			if (leaf)
			{
				if (price == leaf->price)
					return leaf;
				if (price < leaf->price)
					return search(price, leaf->left.get());
				else
					return search(price, leaf->right.get());
			}
			else return nullptr;
		}

		unique_ptr<level_queue> root;
	};
}
#endif