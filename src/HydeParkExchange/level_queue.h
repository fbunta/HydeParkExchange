#ifndef LEVEL_QUEUE_H
#  define LEVEL_QUEUE_H
#include <memory>
#include <mutex>
#include <condition_variable>
#include "order.h";

using std::unique_ptr;
using std::shared_ptr;
using std::make_unique;

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
        unique_ptr<level_queue> left;
        unique_ptr<level_queue> right;
        
        order pop() { // pops from the head of the queue
            const std::lock_guard<std::mutex> lock(mtx);
            if (head == nullptr) {
                throw std::invalid_argument("cannot pop order from empty level");
            }
            order p = head->value; // get head value to return
            this->head = head->next; // set new head to old heads next
            if (this->head != nullptr) {
                this->head->prev = nullptr; // set new head prev to null so memory gets released
            }
            else {
                tail = nullptr; // if we have no head then the tail needs to be cleaned up
            }
            return p;
        };

        //T pop(int) { // for cancellations this pops from the middle of the queue 
        //    std::unique_lock<std::mutex> lock(mtx);
        //};

        void push(order const& item) { // always push to the tail of the queue
            const std::lock_guard<std::mutex> lock(mtx);
            std::shared_ptr<queue_item> new_item = std::make_shared<queue_item>(item);
            if (tail != nullptr) {
                new_item->prev = tail; // set prev to old tail
                tail->next = new_item; // set next old tail to this item
            }
            else {
                head = new_item; // we dont have a head yet so set it here
            }
            tail = new_item; // set this item as new tail
        };

        struct queue_item {
            // the "next" item gets popped out right after a given item
            // the "previous" item gets popped out right before a given item
            queue_item(order x) : prev(nullptr), next(nullptr), value(x) {}
            std::shared_ptr<queue_item> prev; // if prev is nullptr then we assume this item is the head
            std::shared_ptr<queue_item> next; // if next is nullptr then we assume this is the tail
            order value; // this will be the order object
        };

    private:
        std::mutex mtx;
        std::condition_variable cv;
        shared_ptr<queue_item> head;
        shared_ptr<queue_item> tail;
    };
}
#endif