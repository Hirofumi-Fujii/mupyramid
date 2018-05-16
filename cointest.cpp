// cointest.cpp
// g++ -Wall cointest.cpp coinrecord.cpp gzfstream.cpp gzipfilebuf.cpp -lz -o cointest.exe

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "gzfstream.h"
#include "coinrecord.h"

static const unsigned int NUM_UNITS = 3;
static const char* cointype[4] = { "01*", "0*2", "*12", "012" };

int main (int argc, char* argv[])
{
	unsigned long numcoinrec = 0;

	// check the arguments
	if (argc != 2)
	{
		// missing filename. show usage.
		std::cout
			<< "Usage: "
			<< argv[0]
			<< " data_filename"
			<< std::endl;
		return (-1);
	}

	// open the input file
	mylibrary::igzfstream ifs (argv[1]);
	if (!ifs)
	{
		// file open error
		std::cerr
			<< "ERROR: data file ("
			<< argv[1]
			<< ") open error."
			<< std::endl;
		return (-1);
	}

	// read file and count the data-records
	std::string recstr;
	while (getline (ifs, recstr))
	{
		if ((recstr.size() > 4) &&
			(recstr.substr(0,4) == "COIN"))
		{
			++numcoinrec;
			std::stringstream ss (recstr.substr(4));
			MUONDAQ::CoinRecord coinrec;
			if (coinrec.Read (ss))
			{
				if ((coinrec.numdat(0) > 0) &&
					(coinrec.numdat(1) > 0) &&
					(coinrec.numdat(2) > 0))
				{
					std::cout << "X"
						<< ' ' << coinrec.xpos(0)
						<< ' ' << coinrec.xpos(1)
						<< ' ' << coinrec.xpos(2)
						<< " Y"
						<< ' ' << coinrec.ypos(0)
						<< ' ' << coinrec.ypos(1)
						<< ' ' << coinrec.ypos(2)
						<< std::endl;
				}
			}
		}
		else if (recstr.size() > 4)
		{
			if (recstr.substr(0,4) == "RUNS")
				std::cout << recstr << std::endl;
			else if (recstr.substr(0,4) == "RUNE")
				std::cout << recstr << std::endl;
			else if (recstr.substr(0,4) == "PARA")
				std::cout << recstr << std::endl;
		}
	}
	return 0;
}
