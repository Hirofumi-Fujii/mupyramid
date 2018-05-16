// coinanaz.cpp
// g++ -Wall coinanaz.cpp gzfstream.cpp gzipfilebuf.cpp -lz -o coinanaz.exe

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "gzfstream.h"

static const unsigned int NUM_UNITS = 3;
static const char* cointype[4] = { "01*", "0*2", "*12", "012" };

int main (int argc, char* argv[])
{
	unsigned long numcoin [4] = { 0 };
	unsigned long numcoins[4] = { 0 };
	unsigned long numcoinh[4] = { 0 };
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
			std::stringstream ss (recstr);
			std::string recid;
			ss >> recid;
			int numdat[NUM_UNITS] = { 0 };
			int numxhits[NUM_UNITS] = { 0 };
			int numyhits[NUM_UNITS] = { 0 };
			int numxclust[NUM_UNITS] = { 0 };
			int numyclust[NUM_UNITS] = { 0 };
			for (unsigned int u = 0; u < NUM_UNITS; u++)
			{
				int n;
				if (ss >> n)
					numdat[u] = n;
			}
			for (unsigned int u = 0; u < NUM_UNITS; u++)
			{
				int n;
				if (ss >> n)
					numxhits[u] = n;
				if (ss >> n)
					numxclust[u] = n;
				ss >> n;	// Xav
				if (ss >> n)
					numyhits[u] = n;
				if (ss >> n)
					numyclust[u] = n;
				ss >> n;	// Yav
				double d;
				ss >> d;	// microT
				ss >> d;	// nanoT
			}
			if ((numdat[0] > 0) && (numdat[1] > 0))
			{
				numcoin[0] += 1;
				if ((numxclust[0] == 1) && (numyclust[0] == 1)
					&& (numxclust[1] == 1) && (numyclust[1] == 1))
				{
					numcoins[0] += 1;
					if ((numxhits[0] <= 3) && (numyhits[0] <= 3)
						&& (numxhits[1] <= 3) && (numyhits[1] <= 3))
						numcoinh[0] += 1;
				}
			}
			if ((numdat[0] > 0) && (numdat[2] > 0))
			{
				numcoin[1] += 1;
				if ((numxclust[0] == 1) && (numyclust[0] == 1)
					&& (numxclust[2] == 1) && (numyclust[2] == 1))
				{
					numcoins[1] += 1;
					if ((numxhits[0] <= 3) && (numyhits[0] <= 3)
						&& (numxhits[2] <= 3) && (numyhits[2] <= 3))
						numcoinh[1] += 1;
				}
			}
			if ((numdat[1] > 0) && (numdat[2] > 0))
			{
				numcoin[2] += 1;
				if ((numxclust[1] == 1) && (numyclust[1] == 1)
					&& (numxclust[2] == 1) && (numyclust[2] == 1))
				{
					numcoins[2] += 1;
					if ((numxhits[1] <= 3) && (numyhits[1] <= 3)
						&& (numxhits[2] <= 3) && (numyhits[2] <= 3))
						numcoinh[2] += 1;
				}
			}
			if ((numdat[0] > 0) && (numdat[1] > 0) && (numdat[2] > 0))
			{
				numcoin[3] += 1;
				if ((numxclust[0] == 1) && (numyclust[0] == 1)
					&& (numxclust[1] == 1) && (numyclust[1] == 1)
					&& (numxclust[2] == 1) && (numyclust[2] == 1))
				{
					numcoins[3] += 1;
					if ((numxhits[0] <= 3) && (numyhits[0] <= 3)
						&& (numxhits[1] <= 3) && (numyhits[1] <= 3)
						&& (numxhits[2] <= 3) && (numyhits[2] <= 3))
						numcoinh[3] += 1;
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

	// Show results.
	std::cout
		<< "Total COIN records "
		<< numcoinrec
		<< std::endl;

	for (int i = 0; i < 4; i++)
		std::cout
			<< "Type[" << cointype[i] << "] "
			<< numcoin[i]
			<< ' ' << numcoins[i]
			<< ' ' << numcoinh[i]
			<< std::endl;
	return 0;
}
