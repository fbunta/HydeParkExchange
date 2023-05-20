#ifndef ORDER_H
#	define ORDER_H
#include "order_id_singleton.h"
using hpx::order_id_singleton;

namespace hpx {

	enum class OrderType {
		Limit,
		Market,
		IOC
	};

	enum class OrderSide {
		Buy,
		Sell
	};

	enum class OrderStatus {
		Inactive,
		Active,
		Filled,
		Cancelled
	};

	class order {
	public:
		OrderType type_;
		OrderSide side_;
		OrderStatus status_;
		int quantity_;
		float price_;
		int order_id_;
		int user_id_;
		order(OrderType type, OrderSide side, int qty, float price) {
			type_ = type;
			side_ = side;
			status_ = OrderStatus::Inactive;
			quantity_ = qty;
			price_ = price;
			order_id_ = singleton->get_new_order_id();
			user_id_ = 0; // TODO
		}
	private:
		order_id_singleton* singleton = order_id_singleton::get_instance(1);
	};

	struct market_order : public order {
		using order::order;
		market_order(OrderSide side, int qty) : order(OrderType::Market, side, qty, 0) {}
	};

	struct limit_order : public order {
		limit_order(OrderSide side, int qty, float price) : order(OrderType::Limit, side, qty, price) {}
	};

	struct ioc_order : public order {
		ioc_order(OrderSide side, int qty, float price) : order(OrderType::IOC, side, qty, price) {}
	};
}
#endif // !ORDER_H
