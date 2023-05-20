#ifndef LEVEL_H
#	define LEVEL_H
#include "order.h";
#include <memory>

using std::unique_ptr;
using std::shared_ptr;

namespace hpx {

	struct level {
		level() = default;

		level(float price) {
			price_ = price;
		}
		float price_;
		std::unique_ptr<level> left_child_;
		std::unique_ptr<level> right_child_;
		std::shared_ptr<order> head_;
		std::shared_ptr<order> tail_;
	};
}
#endif // !LEVEL_H
