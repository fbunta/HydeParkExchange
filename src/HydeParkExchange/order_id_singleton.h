#ifndef ORDER_ID_SINGLETON_H
#	define ORDER_ID_SINGLETON_H
#include<atomic>
using std::atomic;

namespace hpx {
    // this class generates sequential order id integers in a thread-safe way with atomics
    class order_id_singleton
    {
    protected:
        order_id_singleton(const int starting_id) {
            atomic<int> id = atomic<int>{ starting_id };
        }
        static order_id_singleton* singleton_;
        atomic<int> id;

    public:
        order_id_singleton(order_id_singleton& other) = delete;
        void operator=(const order_id_singleton&) = delete;
        static order_id_singleton* get_instance(const int starting_value);
        int get_new_order_id() {
            return id++;
        }
    };

    order_id_singleton* order_id_singleton::singleton_ = nullptr;

    order_id_singleton* order_id_singleton::get_instance(int starting_id)
    {
        if (singleton_ == nullptr) {
            singleton_ = new order_id_singleton(starting_id);
        }
        return singleton_;
    }
}
#endif
