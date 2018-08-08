// testclock.cpp
//
// test for BBTX036MULTI::XYUnitClock
//
// g++ -Wall testclock.cpp xyunitdata.cpp -o testclock
//

#include <iostream>
#include "xyunitdata.h"

int main (int argc, char * argv[])
{
	BBTX036MULTI::XYUnitClock clk1((unsigned int)(0));
	BBTX036MULTI::XYUnitClock clk2((unsigned int)(10));
	BBTX036MULTI::XYUnitClock clk3((unsigned int)(1000));
	BBTX036MULTI::XYUnitClock clk4((unsigned int)(22000));
	BBTX036MULTI::XYUnitClock clk5((unsigned int)(31700));
	BBTX036MULTI::XYUnitClock clk6((unsigned int)(-30));
	BBTX036MULTI::XYUnitClock clk7((unsigned int)(30));

	BBTX036MULTI::XYUnitClock tck1 = clk3 - clk2;
	std::cout << tck1.microsec() << '.' << tck1.nanosec() << std::endl;
	tck1 = clk5 - clk4;
	std::cout << tck1.microsec() << '.' << tck1.nanosec() << std::endl;
	tck1 = tck1 + clk2;
	std::cout << tck1.microsec() << '.' << tck1.nanosec() << std::endl;
	tck1 = clk5 - clk4 + clk2;
	std::cout << tck1.microsec() << '.' << tck1.nanosec() << std::endl;
	tck1 = clk4 - clk5;
	std::cout << tck1.microsec() << '.' << tck1.nanosec() << std::endl;
	tck1 = tck1 + clk5;
	std::cout << tck1.microsec() << '.' << tck1.nanosec() << std::endl;
	tck1 = clk5 + clk6;
	std::cout << tck1.microsec() << '.' << tck1.nanosec() << std::endl;
	tck1 = clk5 - clk7;
	std::cout << tck1.microsec() << '.' << tck1.nanosec() << std::endl;
	tck1 = clk1 - clk7;
	std::cout << tck1.microsec() << '.' << tck1.nanosec() << std::endl;
	tck1 = clk5 + tck1;
	std::cout << tck1.microsec() << '.' << tck1.nanosec() << std::endl;

	return 0;
}
