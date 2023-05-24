#ifndef ORDER_H
#	define ORDER_H
#include "order_id_singleton.h"
#include <condition_variable>

using hpx::order_id_singleton;

namespace hpx {

	enum class TradingEntity {
		Belvedere,
		Wolverine,
		Citadel,
		IMC
	};

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
		FullFill,
		PartialFill,
		Cancelled,
		Rejected
	};

	class order {
	public:// add cv or future for telling other threads of status
		OrderType type_; //variant of order types and visit it
		OrderSide side_;
		OrderStatus status_;
		int quantity_;
		double price_;
		int order_id_;
		TradingEntity trading_entity_;
		order(OrderType type, OrderSide side, int qty, double price, TradingEntity te) {
			type_ = type;
			side_ = side;
			status_ = OrderStatus::Inactive;
			quantity_ = qty;
			price_ = price;
			order_id_ = singleton->get_new_order_id();
			trading_entity_ = te;
		}

		void fill_order(int qty_filled) {
			quantity_ = quantity_ - qty_filled;
			if (quantity_ == 0) {
				status_ = OrderStatus::FullFill;
			}
			else {
				status_ = OrderStatus::PartialFill;
			}
		}
	private:
		order_id_singleton* singleton = order_id_singleton::get_instance(1);
	};

	struct market_order : public order {
		market_order(OrderSide side, int qty, TradingEntity te) : order(OrderType::Market, side, qty, 0, te) {}
	};

	struct limit_order : public order {
		limit_order(OrderSide side, int qty, double price, TradingEntity te) : order(OrderType::Limit, side, qty, price, te) {}
	};

	struct ioc_order : public order {
		ioc_order(OrderSide side, int qty, double price, TradingEntity te) : order(OrderType::IOC, side, qty, price, te) {}
	};
}
#endif // !ORDER_H
