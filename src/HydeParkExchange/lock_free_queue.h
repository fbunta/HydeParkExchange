#ifndef LOCK_FREE_QUEUE_H
#  define LOCK_FREE_QUEUE_H
#include "order.h"
#include<atomic>
#include<memory>

using std::atomic;


namespace hpx {


    template<typename T>
    class XQueue {
    public:
        XQueue();
        T pop(); // pops from the head of the queue
        T pop(int); // for cancellations this pops from the middle of the queue 

        void push(T const& item) { // always push to the tail of the queue
            atomic<QueueItem*> const new_item = new QueueItem(item);
            QueueItem* old_tail = tail.load();
            do {
                new_item->before = old_tail;
                old_tail->after = new_item;
                // need a counter because could be same location in memory
            } while (!tail.compare_exchange_weak(old_tail, new_item));
            // in compare_exchange_weak if old_tail and new_item are equal then we return true and while loop continues
            // replacing tail with new_item 
        };

        struct QueueItem {
            QueueItem(T x) : before(nullptr), after(nullptr), value(x) {}
            QueueItem* before; // if before is nullptr then we assume this item is the head
            QueueItem* after; // if after is nullptr then we assume this is the tail
            T value; // this will be the order object
        };
    private:
        atomic<QueueItem*> head;
        atomic<QueueItem*> tail;
    };

    XQueue::XQueue()
    {
        StackHead init;
        init.link = nullptr;
        init.count = 0;
        head.store(init);
    }

    int XQueue::pop()
    {
        // What the head will be if nothing messed with it
        StackHead expected = head.load();
        StackHead newHead;
        bool succeeded = false;
        while (!succeeded) {
            if (expected.link == 0) {
                return 0; // List is empty
            }
            // What the head will be after the pop:
            newHead.link = expected.link->next;
            newHead.count = expected.count + 1;
            // Even if the compare_exchange fails, it updates expected.
            succeeded = head.compare_exchange_weak(expected, newHead);
        }
        int value = expected.link->value;
        delete expected.link;
        return value;
    }

    // Push an item onto the list with the given head
    void XQueue::push(order val)
    {
        StackHead expected = head.load();
        QueueItem* newItem = new QueueItem(val);
        StackHead newHead;
        newHead.link = newItem;
        do {
            newItem->next = expected.link;
            newHead.count = expected.count + 1;
        } while (!head.compare_exchange_weak(expected, newHead));
    }
}
#endif