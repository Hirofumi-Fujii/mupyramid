// cointest2.cpp
// g++ -Wall cointest2.cpp coinrecord.cpp hist2d.cpp gzfstream.cpp gzipfilebuf.cpp -lz -o cointest2.exe

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "gzfstream.h"
#include "coinrecord.h"
#include "hist2d.h"

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

	mylibrary::Hist2D hist2d00 ("dxdy(0-2)", -99.5, 99.5, 199, -99.5, 99.5, 199);
	mylibrary::Hist2D hist2d01 ("dxdy(center)", -99.5, 99.5, 199, -99.5, 99.5, 199);

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
					int dx = (990 - coinrec.xpos(0)) - coinrec.xpos(2);
					int dy = coinrec.ypos(0) - coinrec.ypos(2);
					hist2d00.cumulate((double)(dx) / 10.0, (double)(dy) / 10.0);

					int xexp = ((990 - coinrec.xpos(0)) + coinrec.xpos(2)) / 2;
					int yexp = (coinrec.ypos(0) + coinrec.ypos(2)) / 2;

					int ddx = (990 - coinrec.xpos(1)) - xexp;
					int ddy = coinrec.ypos(1) - yexp;
					hist2d01.cumulate((double)(ddx) / 10.0, (double)(ddy) / 10.0);
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
	hist2d00.dump(std::cout);
	hist2d01.dump(std::cout);
	return 0;
}
