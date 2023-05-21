
#include "single_asset_book.h"
#include "traders.h"
#include <iostream>
#include <memory>
#include <string>
#include <thread>

using namespace hpx;
using namespace std;



int main()
{
	single_asset_book exchange = single_asset_book();
	jthread t1(send_orders_1, ref(exchange));
	//jthread t2(send_orders_2, ref(exchange));
	//jthread t3(pop_orders_1, ref(exchange));
	t1.join();
	//t2.join();

	return 0;

}

