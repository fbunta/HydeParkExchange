#ifndef LEVEL_QUEUE_H
#  define LEVEL_QUEUE_H
#include "order.h"
#include "fill.h"
#include <mutex>
#include <condition_variable>
#include <coroutine>

using std::unique_ptr;
using std::shared_ptr;
using std::make_shared;
using std::make_unique;
using std::mutex;
using std::atomic;
using std::lock_guard;
using std::scoped_lock;

namespace hpx {
    //template<typename T> // requires numeric type trait
    class level_queue {
    public:
        level_queue(double price) : price(price) {}

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

        double price;
        atomic<int> buy_size{0};
        atomic<int> sell_size{0};
        unique_ptr<level_queue> left;
        unique_ptr<level_queue> right;
        
        // pops from the head of the queue
        unique_ptr<fill> pop() {
            scoped_lock lock_both(buy_mtx, sell_mtx);
            if (buy_head == nullptr || sell_head == nullptr) {
                return nullptr;
            }
            int filled_qty = std::min(buy_head->value->quantity_, sell_head->value->quantity_);
            unique_ptr<fill> f = make_unique<fill>(buy_head->value->order_id_, buy_head->value->trading_entity_,
                sell_head->value->order_id_, sell_head->value->trading_entity_, this->price, filled_qty);
            buy_head->value->fill_order(filled_qty);
            sell_head->value->fill_order(filled_qty);
            buy_size -= filled_qty;
            sell_size -= filled_qty;

            if (buy_head->value->status_ == order_status::FullFill) {
                buy_head = buy_head->next; // set new head to old heads next
                if (buy_head != nullptr) {
                    buy_head->prev = nullptr; // set new head prev to null so memory gets released
                }
                else {
                    buy_tail = nullptr; // if we have no head then the tail needs to be cleaned up
                }
            }
            if (sell_head->value->status_ == order_status::FullFill) {
                sell_head = sell_head->next;
                if (sell_head != nullptr) {
                    sell_head->prev = nullptr;
                }
                else {
                    sell_tail = nullptr;
                }
            }
            return f;
        };

        // for cancellations this pops from the middle of the queue
        void buy_pop(int order_id) {
            const lock_guard<mutex> lock(buy_mtx);
            if (buy_head == nullptr) {
                throw std::invalid_argument("nothing to cancel");
            }
            shared_ptr<queue_item> item_to_check = buy_head;
            while (item_to_check != nullptr)
            {
                if (item_to_check->value->order_id_ == order_id)
                {
                    if (buy_tail == item_to_check) {
                        buy_tail = item_to_check->prev;
                    }
                    if (buy_head == item_to_check) {
                        buy_head= item_to_check->next;
                    }
                    if (item_to_check->prev != nullptr) {
                        item_to_check->prev->next = item_to_check->next;
                    }
                    if (item_to_check->next != nullptr) {
                        item_to_check->next->prev = item_to_check->prev;
                    }
                    item_to_check->value->status_ = order_status::Cancelled;;
                    buy_size -= item_to_check->value->quantity_;
                    return;
                }
                else {
                    item_to_check = item_to_check->next;
                }
            }
        };

        // for cancellations this pops from the middle of the queue
        void sell_pop(int order_id) {
            const lock_guard<mutex> lock(sell_mtx);
            if (sell_head == nullptr) {
                throw std::invalid_argument("nothing to cancel");
            }
            shared_ptr<queue_item> item_to_check = sell_head;
            while (item_to_check != nullptr)
            {
                if (item_to_check->value->order_id_ == order_id)
                {
                    if (sell_tail == item_to_check) {
                        sell_tail = item_to_check->prev;
                    }
                    if (sell_head == item_to_check) {
                        sell_head = item_to_check->next;
                    }
                    if (item_to_check->prev != nullptr) {
                        item_to_check->prev->next = item_to_check->next;
                    }
                    if (item_to_check->next != nullptr) {
                        item_to_check->next->prev = item_to_check->prev;
                    }
                    item_to_check->value->status_ = order_status::Cancelled;;
                    sell_size -= item_to_check->value->quantity_;
                    return;
                }
                else {
                    item_to_check = item_to_check->next;
                }
            }
        };

        void push(unique_ptr<order> new_order) { // always push to the tail of the queue
            if ( side_to_string(new_order->side_) == "buy") {
                const lock_guard<mutex> lock(buy_mtx);
                shared_ptr<queue_item> new_item = make_shared<queue_item>(move(new_order));

                if (buy_tail) { // there is an existing linked order list
                    buy_tail->next = new_item;
                    new_item->prev = buy_tail;
                }
                else { // the linked order list is empty
                    buy_head = new_item;
                }
                buy_tail = new_item;
                buy_size += new_item->value->quantity_;
                new_item->value->status_ = order_status::Active;
            }
            else {
                const lock_guard<mutex> lock(sell_mtx);
                shared_ptr<queue_item> new_item = make_shared<queue_item>(move(new_order));
                if (sell_tail) { // there is an existing linked order list
                    new_item->prev = sell_tail; // set prev to old tail
                    sell_tail->next = new_item; // set next old tail to this item
                }
                else {
                    sell_head = new_item; // we dont have a head yet so set it here
                }
                sell_tail = new_item; // set this item as new tail
                sell_size += new_item->value->quantity_;
                new_item->value->status_ = order_status::Active;
            }
        };

        // the "next" item gets popped out right after a given item
        // the "previous" item gets popped out right before a given item
        struct queue_item {
            queue_item(unique_ptr<order> ord) : prev(nullptr), next(nullptr), value(std::move(ord)) {}
            shared_ptr<queue_item> prev; // if prev is nullptr then we assume this item is the head
            shared_ptr<queue_item> next; // if next is nullptr then we assume this is the tail
            unique_ptr<order> value; // this will be the order object
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
