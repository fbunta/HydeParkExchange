#ifndef ORDER_H
#	define ORDER_H
#include "order_id_singleton.h"
#include <condition_variable>
#include <ostream>
#include <format>
#include <string>

using fmt::format;
using std::string;
using std::ostream;
using hpx::order_id_singleton;

namespace hpx {

	enum class trading_entity {
		Belvedere,
		Wolverine,
		Citadel,
		IMC
	};

	string entity_to_string(trading_entity entity)
	{
		switch (entity) {
		case(trading_entity::Belvedere): return "Belvedere";
		case(trading_entity::Wolverine): return "Wolverine";
		case(trading_entity::Citadel): return "Citadel";
		case(trading_entity::IMC): return "IMC";
		}
	};

	enum class order_type {
		Limit,
		Market,
		IOC
	};

	string order_type_to_string(order_type ord_type)
	{
		switch (ord_type) {
		case(order_type::Limit): return "limit";
		case(order_type::Market): return "market";
		case(order_type::IOC): return "ioc";
		}
	};

	enum class order_side {
		Buy,
		Sell
	};

	string side_to_string(order_side side)
	{
		switch (side) {
		case(order_side::Buy): return "buy";
		case(order_side::Sell): return "sell";
		}
	};

	enum class order_status {
		Inactive,
		Active,
		FullFill,
		PartialFill,
		Cancelled,
		Rejected
	};

	string order_status_to_string(order_status status)
	{
		switch (status) {
			case(order_status::Inactive): return "inactive";
			case(order_status::Active): return "active";
			case(order_status::FullFill): return "fullfill";
			case(order_status::PartialFill): return "partialfill";
			case(order_status::Cancelled): return "cancelled";
			case(order_status::Rejected): return "rejected";
		}
	}

	class order {
	public:
		order_type type_; //variant of order types and visit it
		order_side side_;
		order_status status_;
		int quantity_;
		double price_;
		int order_id_;
		trading_entity trading_entity_;
		order(order_type type, order_side side, int qty, double price, trading_entity te) {
			type_ = type;
			side_ = side;
			status_ = order_status::Inactive;
			quantity_ = qty;
			price_ = price;
			order_id_ = singleton->get_new_order_id();
			trading_entity_ = te;
		}

		void fill_order(int qty_filled) {
			quantity_ = quantity_ - qty_filled;
			if (quantity_ == 0) {
				status_ = order_status::FullFill;
			}
			else {
				status_ = order_status::PartialFill;
			}
		}
	private:
		order_id_singleton* singleton = order_id_singleton::get_instance(1);
	};

	inline ostream&
	operator<<(ostream& os, order ord)
	{
		os << format("{}={} {} {} {} @ {}", ord.order_id_, order_status_to_string(ord.status_)
			, entity_to_string(ord.trading_entity_), side_to_string(ord.side_), ord.quantity_, ord.price_);
		return os;
	}

	struct market_order : public order {
		market_order(order_side side, int qty, trading_entity te) : order(order_type::Market, side, qty, 0, te) {}
	};

	struct limit_order : public order {
		limit_order(order_side side, int qty, double price, trading_entity te) : order(order_type::Limit, side, qty, price, te) {}
	};

	struct ioc_order : public order {
		ioc_order(order_side side, int qty, double price, trading_entity te) : order(order_type::IOC, side, qty, price, te) {}
	};
}
#endif // !ORDER_H
