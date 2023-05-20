
#include "btree.h"
#include "traders.h"
#include <iostream>
#include <memory>
#include <string>
#include <thread>

using namespace hpx;
using namespace std;



int main()
{
	level_btree exchange = level_btree();
	jthread t1(send_orders_1, ref(exchange));
	//jthread t2(send_orders_2, ref(exchange));
	//jthread t3(pop_orders_1, ref(exchange));
	t1.join();
	//t2.join();

	return 0;

}

