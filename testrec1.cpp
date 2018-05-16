// testrec1.cpp
//
// simple test for raw data file
//
// g++ -Wall testrec1.cpp  gzfstream.cpp gzipfilebuf.cpp -lz -o testrec1
//

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "gzfstream.h"

double dathist[256];
int runscount = 0;
int runecount = 0;
std::string runsstr = std::string("");
std::string runestr = std::string("");

int main (int argc, char* argv[] )
{
	// open the input file
	mylibrary::igzfstream ifs (argv[1]);
	if (!ifs)
	{
		std::cerr << "ERROR: input file ("
			<< argv[1]
			<< ") open error."
			<< std::endl;
		return (-1);
	}

	// Clear the flags
	runscount = 0;
	runecount = 0;

	// Clear the histogram
	double totaldat = 0.0;
	for (int i = 0; i < 256; i++)
		dathist[i] = 0.0;

	std::string sline;
	while (getline (ifs, sline))
	{
		if (sline.size () > 4)
		{
			if (sline.substr (0, 3) == "DAT")
			{
				totaldat += 1.0;
				char c = sline [3];
				int unitno = (int)(c) & 255;
				dathist[unitno] += 1.0;
			}
			else if (sline.substr (0, 4) == "RUNS")
			{
				runscount += 1;
				runsstr = sline;
			}
			else if (sline.substr (0, 4) == "RUNE")
			{
				runecount += 1;
				runestr = sline;
			}
		}
	}
	
	// Dump the information
	if (runscount == 0)
		std::cout << "RUNS record missing" << std::endl;
	else
		std::cout << runsstr << std::endl;
	if (runecount == 0)
		std::cout << "RUNE record missing" << std::endl;
	else
		std::cout << runestr << std::endl;

	std::cout.precision(20);
	std::cout << "Total DAT record," << totaldat << std::endl;
	for (int i = 0; i < 256; i++)
	{
		if (dathist[i] > 0.0)
		{
			char c = (char)(i & 255);
			std::cout << c << ',' << dathist[i] << std::endl;
		}
	}
	return 0;
}
