#ifndef LEVEL_QUEUE_H
#  define LEVEL_QUEUE_H
#include <memory>
#include <mutex>
#include <condition_variable>
#include "order.h";
#include <iostream>

using std::unique_ptr;
using std::shared_ptr;
using std::make_shared;
using std::make_unique;
using std::mutex;
using std::atomic;
using std::lock_guard;
using std::cout;
using std::endl;

namespace hpx {
    //template<typename T> // requires numeric type trait
    class level_queue {
    public:
        level_queue(float price) : price(price) {}

        level_queue(level_queue const& other) : price(other.price) {
            if (other.left)
                left = make_unique<level_queue>(*other.left);
            if (other.right)
                right = make_unique<level_queue>(*other.right);
        }
        
        level_queue& operator=(level_queue const& other) {
            price = other.price;
            if (other.left)
                left = make_unique<level_queue>(*other.left);
            if (other.right)
                right = make_unique<level_queue>(*other.right);
            return *this;
        }

        float price;
        atomic<int> buy_size{0};
        atomic<int> sell_size{0};
        unique_ptr<level_queue> left;
        unique_ptr<level_queue> right;
        
        void pop() { // pops from the head of the queue
            const lock_guard<mutex> lock_buy(buy_mtx);
            const lock_guard<mutex> lock_sell(sell_mtx);
            if (buy_head == nullptr) {
                return;
            }
            if (sell_head == nullptr) {
                return;
            }
            cout << "We got a fill!" << endl;
            int filled_qty = std::min(buy_head->value->quantity_, sell_head->value->quantity_);
            buy_head->value->fill(filled_qty);
            sell_head->value->fill(filled_qty);
            buy_size -= buy_head->value->quantity_;
            sell_size -= sell_head->value->quantity_;
            buy_head = buy_head->next; // set new head to old heads next
            if (buy_head != nullptr) {
                buy_head->prev = nullptr; // set new head prev to null so memory gets released
            }
            else {
                buy_tail = nullptr; // if we have no head then the tail needs to be cleaned up
            }
            sell_head = sell_head->next;
            if (sell_head != nullptr) {
                sell_head->prev = nullptr;
            }
            else {
                buy_tail = nullptr;
            }
        };

        // TODO cancel for sell side
        order* pop(int order_id) { // for cancellations this pops from the middle of the queue 
            const lock_guard<mutex> lock(buy_mtx);
            if (buy_head == nullptr) {
                throw std::invalid_argument("nothing to cancel");
            }
            shared_ptr<queue_item> item_to_check = buy_head;
            while (item_to_check != nullptr)
            {
                if (item_to_check->value->order_id_ == order_id)
                {
                    if (item_to_check->next) {
                        item_to_check->next->prev = item_to_check->prev;
                        item_to_check->prev->next = item_to_check->next;
                        item_to_check->next = nullptr;
                        item_to_check->prev = nullptr;
                    }
                    item_to_check->value->status_ = OrderStatus::Cancelled;;
                    buy_size -= item_to_check->value->quantity_;
                    return item_to_check->value;
                }
                else {
                    item_to_check = item_to_check->next;
                }
            }
        };

        void push(unique_ptr<order>& new_order) { // always push to the tail of the queue
            if (new_order->side_ == OrderSide::Buy) {
                const lock_guard<mutex> lock(buy_mtx);
                shared_ptr<queue_item> new_item = make_shared<queue_item>(new_order.get());
                if (buy_tail != nullptr) {
                    new_item->prev = buy_tail; // set prev to old tail
                    buy_tail->next = new_item; // set next old tail to this item
                }
                else {
                    buy_head = new_item; // we dont have a head yet so set it here
                }
                buy_tail = new_item; // set this item as new tail
                buy_size += new_order->quantity_;
                new_order->status_ = OrderStatus::Active;
            }
            else {
                const lock_guard<mutex> lock(sell_mtx);
                shared_ptr<queue_item> new_item = make_shared<queue_item>(new_order.get());
                if (sell_tail != nullptr) {
                    new_item->prev = sell_tail; // set prev to old tail
                    sell_tail->next = new_item; // set next old tail to this item
                }
                else {
                    sell_head = new_item; // we dont have a head yet so set it here
                }
                sell_tail = new_item; // set this item as new tail
                sell_size += new_order->quantity_;
                new_order->status_ = OrderStatus::Active;
            }
        };

        struct queue_item {
            // the "next" item gets popped out right after a given item
            // the "previous" item gets popped out right before a given item
            queue_item(order* ord) : prev(nullptr), next(nullptr), value(ord) {}
            shared_ptr<queue_item> prev; // if prev is nullptr then we assume this item is the head
            shared_ptr<queue_item> next; // if next is nullptr then we assume this is the tail
            order* value; // this will be the order object
        };

    private:
        mutex buy_mtx;
        mutex sell_mtx;
        shared_ptr<queue_item> sell_head;
        shared_ptr<queue_item> sell_tail;
        shared_ptr<queue_item> buy_head;
        shared_ptr<queue_item> buy_tail;
    };
}
#endif