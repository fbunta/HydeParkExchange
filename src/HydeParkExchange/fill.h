#ifndef FILL_H
#	define FILL_H

namespace hpx {

	class fill {
	public:
		int buy_order_id_;
		int sell_order_id_;
		double price_;
		int qty_;
		fill(int buy_order_id, int sell_order_id, double price, int qty) {
			buy_order_id_ = buy_order_id;
			sell_order_id_ = sell_order_id;
			price_ = price;
			qty_ = qty;
		}
	};
}
#endif // !FILL_H
