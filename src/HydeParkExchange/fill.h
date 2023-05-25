#ifndef FILL_H
#	define FILL_H
#include <format>
#include <ostream>

using std::format;
using std::string;
using std::ostream;

namespace hpx {

	struct buy_fill {
		int id_;
		trading_entity entity_;
		double price_;
		int qty_;
		buy_fill(int id, trading_entity e, double p, int q) : id_(id), entity_(e), price_(p), qty_(q) {}
	};

	struct sell_fill {
		int id_;
		trading_entity entity_;
		double price_;
		int qty_;
		sell_fill(int id, trading_entity e, double p, int q) : id_(id), entity_(e), price_(p), qty_(q) {}
	};

	struct fill {
	public:
		buy_fill buy;
		sell_fill sell;
		fill(int buy_order_id, trading_entity buy_entity, int sell_order_id, trading_entity sell_entity, double price, int qty) :
			buy(buy_order_id, buy_entity, price, qty),
			sell(sell_order_id, sell_entity, price, qty) 
		{}
	};

	inline ostream&
	operator<<(ostream& os, buy_fill f)
	{
		os << format(" {} Fill {} BUY {} @ {}", entity_to_string(f.entity_), f.id_ , f.qty_, f.price_);
		return os;
	}

	inline ostream&
	operator<<(ostream& os, sell_fill f)
	{
		os << format(" {} Fill {} SELL {} @ {}", entity_to_string(f.entity_), f.id_, f.qty_, f.price_);
		return os;
	}
}
#endif // !FILL_H
