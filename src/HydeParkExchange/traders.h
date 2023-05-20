#ifndef TRADERS_H
#  define TRADERS_H
#include "lock_queue.h"
#include <iostream>
#include <chrono>
#include <utility>

using std::cout;
using std::endl;
using std::chrono::milliseconds;

namespace hpx {

	void send_orders_1(ex_queue<int>& q) {
		q.push(1);
		std::this_thread::sleep_for(milliseconds(11));
		q.push(2);
		std::this_thread::sleep_for(milliseconds(10));
		q.push(3);
		std::this_thread::sleep_for(milliseconds(9));
		q.push(4);
		std::this_thread::sleep_for(milliseconds(10));
		q.push(5);
	}

	void send_orders_2(ex_queue<int>& q) {
		q.push(5);
		std::this_thread::sleep_for(milliseconds(9));
		q.push(6);
		std::this_thread::sleep_for(milliseconds(10));
		q.push(7);
		std::this_thread::sleep_for(milliseconds(9));
		q.push(8);
		std::this_thread::sleep_for(milliseconds(12));
		q.push(9);
	}

	void pop_orders_1(ex_queue<int>& q) {
		std::this_thread::sleep_for(milliseconds(40));
		cout << q.pop() << endl;
		cout << q.pop() << endl;
		cout << q.pop() << endl;
		cout << q.pop() << endl;
		cout << q.pop() << endl;
		cout << q.pop() << endl;
		cout << q.pop() << endl;
		cout << q.pop() << endl;
		cout << q.pop() << endl;
		cout << q.pop() << endl;
		cout << q.pop() << endl;
		cout << q.pop() << endl;
	}
}
#endif